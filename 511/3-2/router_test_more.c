#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include <assert.h>
#include <sstream>
#include <cstring>
#undef NDEBUG



int main (void)
{
	int rc;
    fprintf (stderr, "test_send_more running...\n");

void *context = zmq_ctx_new();
  //creating sockets
	void *sender = zmq_socket (context, ZMQ_ROUTER);
	assert (sender);
	void *receiver = zmq_socket (context, ZMQ_ROUTER);
	assert (receiver);
	void * router = zmq_socket (context, ZMQ_ROUTER);
	assert (router);

	//socket names
	char * routerName = "router";
	char * senderName = "sender";
	char * recvName = "receiv"; 

	//giving names to sockets
	rc = zmq_setsockopt (router, ZMQ_IDENTITY, routerName, 6*sizeof(char));
  assert (rc == 0);
	rc = zmq_setsockopt (sender, ZMQ_IDENTITY, senderName, 6*sizeof(char));
  assert (rc == 0);
	rc = zmq_setsockopt (receiver, ZMQ_IDENTITY, recvName, 6*sizeof(char));
  assert (rc == 0);

	//setting rcv timeout
	int timeout = 1000; //1s
	rc = zmq_setsockopt (router, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
  assert (rc == 0);
	rc = zmq_setsockopt (sender, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
  assert (rc == 0);
	rc = zmq_setsockopt (receiver, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
  assert (rc == 0);

	//conecting sockets
	rc = zmq_bind (router, "tcp://127.0.0.1:10006");
  assert (rc == 0);
	rc = zmq_connect (sender, "tcp://127.0.0.1:10006");
  assert (rc == 0);
	rc = zmq_connect (receiver, "tcp://127.0.0.1:10006");
  assert (rc == 0);

//waitng for connect
zmq_sleep(1);
/////////////////////////////////////////
	//send msgs
	 // Send 10 messages(3 parts each), all should be routed to the connected pipe
    for (int i = 0; i < 10; ++i)
    {
        //std::string receiver(routerName);
				//printf("Receiver size: %d, data: %s", receiver.size(), receiver.data());
        rc = zmq_send (sender, routerName, 6, ZMQ_SNDMORE);
        assert (rc >= 0);
				rc = zmq_send (sender, recvName, 6, ZMQ_SNDMORE);
        assert (rc >= 0);
				std::stringstream ss;
				ss << "message " << i;
				std::string message = ss.str();
				rc = zmq_send (sender, message.data(), message.size(), 0);
        assert (rc >= 0);
    }

	//waiting for messages
	zmq_sleep(1);

		char buffer[16];

		int seen = 0;
		int more;
		size_t more_size = sizeof(more);

//receiving messages (10 messages with 3 parts each)
    for (int i = 0; i < 10; ++i)
    {
				int parts = 0;
				bool hasWhoSend = 0; 
				while(1){
		      memset (&buffer, 0, sizeof(buffer));
		     	rc  = zmq_recv (router, &buffer, sizeof(buffer), ZMQ_DONTWAIT);
					//checking if more to come
					more = zmq_getsockopt(router, ZMQ_RCVMORE, &more, &more_size);
		      if( rc == -1){
						printf("Did not get %d message, part: %d\n", i, parts);
						break;
					}
					printf("Received: %s\n", buffer);
					parts++;
					if(hasWhoSend == false){
						hasWhoSend = true;		
						continue;			
					}

					//int mode = more > 0? ZMQ_SNDMORE : 0;
					//rc = zmq_send(router, buffer, rc, mode);
					//assert(rc >= 0);
					if(more == 0){
						printf("End of message, got %d parts\n", parts);
						assert(parts == 3);
						seen++;
						break;
					}
					
        }
    }
		printf("Recceived msgs: %d\n", seen);
		assert(seen == 10);




//////////////////////////////////
//close all
	rc = zmq_close (sender);
    assert (rc == 0);
    rc = zmq_close (receiver);
    assert (rc == 0);
 	rc = zmq_close (router);
    assert (rc == 0);

    rc = zmq_ctx_destroy(context);
    assert (rc == 0);


	return 0;
}
