//
// Main thread: XPUB ( ZMQ_XPUB_VERBOSE )
// thrfn1: XSUB/XPUB device ( ZMQ_XPUB_VERBOSE )
// thrfn2 (x2): XSUB 
//
// Test:
// main thread subscribes to the same topic twice
// thrfn2 will only receive one of them (unpatched) or both (patched)
//
// Patch description: in xsub.cpp, zmq::xsub_t::xsend, do not filter
// out duplicate subscription.  It will be filtered out anyway on the
// xpub side, when ZMQ_XPUB_VERBOSE is not used.
//

#include <pthread.h>
#include <zmq.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
void *context;
int count = 0;

// util
void s_relay(void *dst, void *src)
{
    zmq_msg_t message;
    zmq_msg_init (&message);
    zmq_msg_recv (&message, src, 0);
    zmq_msg_send (&message, dst, 0);
    zmq_msg_close (&message);
}


// device
void thrfn1(void *ptr)
{
	void *pub = zmq_socket(context,ZMQ_XPUB);
	int opt = 1;
	zmq_setsockopt(pub,ZMQ_XPUB_VERBOSE,&opt,sizeof(int));
	zmq_bind( pub, "inproc://devpub");

	void *sub = zmq_socket(context,ZMQ_XSUB);
	zmq_connect( sub, "inproc://devsub");

	zmq_pollitem_t items[2];
	items[0].socket = sub;
	items[0].events = ZMQ_POLLIN;
	items[1].socket = pub;
	items[1].events = ZMQ_POLLIN;
	while(1)
	{
		zmq_poll(items,2,-1);
		if( items[0].revents && ZMQ_POLLIN )
			s_relay(pub,sub);
		if( items[1].revents && ZMQ_POLLIN )
			s_relay(sub,pub);
	}
}

// xsub
void thrfn2(void *ptr)
{
	void *socket = zmq_socket(context,ZMQ_XSUB);
	int rc = zmq_connect( socket, "inproc://devpub");
	assert(!rc);

	zmq_msg_t message;
	zmq_msg_init_size (&message, 6);
	memcpy (zmq_msg_data (&message), "\001TOPIC", 6);
	rc = zmq_msg_send (&message, socket, 0);
	zmq_msg_close (&message);

	zmq_pollitem_t items[1];
	items[0].socket = socket;
	items[0].events = ZMQ_POLLIN;

	zmq_poll (items, 1, ptr ? 100 /*100ms*/ : -1);

	switch(count)
	{
	case 1:
		fprintf(stderr,"zmq is UNPATCHED - only one subscription received despite ZMQ_XPUB_VERBOSE\n");
		exit(0);
		break;
	case 2:
		fprintf(stderr,"zmq is PATCHED - both subscriptions received with ZMQ_XPUB_VERBOSE\n");
		exit(0);
		break;
	case 0:
	default:
		fprintf(stderr,"ERROR: %d subscriptions received, should be 1 or 2\n", count);
		exit(-1);
		break;
	}
}

// xpub
int main()
{
	context = zmq_init(0);
	void *pub = zmq_socket(context,ZMQ_XPUB);
	int opt = 1;
	zmq_setsockopt(pub,ZMQ_XPUB_VERBOSE,&opt,sizeof(int));
	zmq_bind( pub, "inproc://devsub");

	pthread_t thread1, thread2, thread2b;
	pthread_create(&thread1,NULL,(void*)&thrfn1,NULL);
	pthread_create(&thread2,NULL,(void*)&thrfn2,NULL);
	pthread_create(&thread2b,NULL,(void*)&thrfn2,(void *)1 /*reporter*/);

	zmq_pollitem_t items[1];
	items[0].socket = pub;
	items[0].events = ZMQ_POLLIN;
	int i = 0;
	for(;;i++)
	{
		zmq_poll(items,1,-1);
		if( items[0].revents && ZMQ_POLLIN )
		{
			fprintf(stderr,"got subscription %d\n", i);
			zmq_msg_t message;
			zmq_msg_init (&message);
			zmq_msg_recv (&message, pub, 0);
			zmq_msg_close(&message);
			count ++;
		}
	}

	return 0;
}
