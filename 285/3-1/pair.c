#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>

void my_free(void* data, void* hint)
{
	free(data);
}

int
main()
{
	void* c = zmq_init(2);
	void* s1 = zmq_socket(c, ZMQ_PAIR);
	void* s2 = zmq_socket(c, ZMQ_PAIR);

	zmq_bind(s1, "inproc://test");
	zmq_connect(s2, "inproc://test");

	void* data = malloc(8);
	assert(data);
	memcpy(data, "testPAIR", 8);

	zmq_msg_t msg;
	int rc = zmq_msg_init_data(&msg, data, 8, my_free, NULL);
	assert(rc == 0);

	if (zmq_sendmsg(s2, &msg, 0))
		perror(zmq_strerror(zmq_errno()));

	zmq_msg_close(&msg);

	rc = zmq_msg_init(&msg);
	assert(rc == 0);

	if (zmq_recvmsg(s1, &msg, 0))
		perror(zmq_strerror(zmq_errno()));

	assert(zmq_close(s1) == 0);
	assert(zmq_close(s2) == 0);
	assert(zmq_term(c) == 0);
}
