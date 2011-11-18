#include <stdio.h>
#include <zmq.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

void * worker(void *ctx) {
	int64_t more;
	size_t more_size = sizeof more;
	void *s = zmq_socket(ctx, ZMQ_ROUTER);
	assert(s);
	assert(zmq_bind(s, "inproc://sock") == 0);
	printf("Worker ready\n");

	long n=0;
	while(1) {
		zmq_msg_t addr;
		zmq_msg_t empty;
		zmq_msg_t msg;
		zmq_msg_init(&addr);
		zmq_msg_init(&empty);
		zmq_msg_init(&msg);

	  	assert(zmq_recv(s, &addr, 0) == 0);
		assert(zmq_getsockopt(s, ZMQ_RCVMORE, &more, &more_size) == 0);
		assert(more == 1);

	  	assert(zmq_recv(s, &empty, 0) == 0);
		assert(zmq_getsockopt(s, ZMQ_RCVMORE, &more, &more_size) == 0);
		assert(more == 1);

	  	assert(zmq_recv(s, &msg, 0) == 0);
		assert(zmq_getsockopt(s, ZMQ_RCVMORE, &more, &more_size) == 0);
		assert(more == 0);

		if(++n % 100 == 0)
			printf("pong %d\n", n);
		assert(zmq_send(s, &addr, ZMQ_SNDMORE) == 0);
		assert(zmq_send(s, &empty, ZMQ_SNDMORE) == 0);
		assert(zmq_send(s, &msg, 0) == 0);

		zmq_msg_close (&addr);
		zmq_msg_close (&msg);
	}
}
void* hammer(void *ctx) {
	while(1) {
		zmq_msg_t msg;
		void *s = zmq_socket(ctx, ZMQ_REQ);
		if(!s) {
			printf("zmq_socket failed %d: %s\n", errno, strerror(errno));
			assert(s);
		}

		assert(zmq_connect(s, "inproc://sock") == 0);

		zmq_msg_init(&msg);
		assert(zmq_msg_init_size (&msg, 6) == 0);
		memset (zmq_msg_data (&msg), 'A', 6);

		assert(zmq_send(s, &msg, 0) == 0);
		zmq_msg_close (&msg);

		zmq_msg_init(&msg);
	  	assert(zmq_recv(s, &msg, 0) == 0);
		zmq_msg_close (&msg);

		zmq_close(s);
	}
}



int main() {
	void *ctx = zmq_init(1);
	pthread_t workerT, hammerT1, hammerT2;

	pthread_create(&workerT, NULL, worker, ctx);
	sleep(1);

	pthread_create(&hammerT1, NULL, hammer, ctx);
	pthread_create(&hammerT2, NULL, hammer, ctx);

	pthread_join(workerT, NULL);

	zmq_term(ctx);
}

