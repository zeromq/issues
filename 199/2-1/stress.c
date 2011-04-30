
#include <zmq.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>


#include <sys/types.h>
#include <unistd.h>

int zsend(void *socket, const char *buffer, const size_t len, char more);
int zsend(void *socket, const char *buffer, const size_t len, char more) {
	assert(socket);
	assert(len > 0);
	assert(buffer);

	zmq_msg_t m;
	assert(0 == zmq_msg_init_size(&m, len));
	void *m_data = zmq_msg_data(&m);
	assert(m_data);
	memcpy(m_data, buffer, len);

	if (0 > zmq_send(socket, &m, more?ZMQ_SNDMORE:0)) {
		int e = zmq_errno();
		fprintf(stderr, "failed to send. errno: %i (%s)\n", e, zmq_strerror(e));
		return 1;
	}
	assert(0 == zmq_msg_close(&m));
	return 0;
}

/*
 * send out a multi-part message of the form:
 * [
 * '####stress',
 * '####AAAAAAAA',
 * '####BBBBBBBB',
 * '####CCCCCCCC',
 * '####DDDDDDDD',
 * '####EEEEEEEE'
 * ]
 *
 * where #### is a prefix for this mp-message
 * passed as argument (\0-terminated; \0 not sent)
 *
 * and because we are using a XREQ socket,
 * there will be an identity message at the beginning.
 *
 */
int stress(void *s, const char *prefix) {
	char buff[32];
	int plen;

	assert(prefix);
	plen = strlen(prefix);
	assert(plen < 20);

	memcpy(buff, prefix, plen);

	memcpy(buff+plen, "stress", 6);
	if (zsend(s, buff, plen+6, 1)) {
		fprintf(stderr, "failed to send initial stress\n");
		return 1;
	}

	memcpy(buff+plen, "AAAAAAAA", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send AAAAAAAA\n");
		return 1;
	}

	memcpy(buff+plen, "BBBBBBBB", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send BBBBBBBB\n");
		return 1;
	}

	memcpy(buff+plen, "CCCCCCCC", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send CCCCCCCC\n");
		return 1;
	}

	memcpy(buff+plen, "DDDDDDDD", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send DDDDDDDD\n");
		return 1;
	}

	memcpy(buff+plen, "EEEEEEEE", 8);
	if (zsend(s, buff, plen+8, 0)) {
		fprintf(stderr, "failed to send EEEEEEEE\n");
		return 1;
	}

	return 0;
}


int main(int argc, char **argv)
{
	if (argc != 1) return 1;

	// assert we have asserts
	int assert_test = 0;
	assert(assert_test = 1);
	if (assert_test != 1) return 1;

	void *context;
	context = zmq_init(1);
	if (!context) return 2;

	void *socket;
	socket = zmq_socket(context, ZMQ_XREQ);
	if (!socket) return 3;

	int ret = zmq_connect(socket, "tcp://localhost:5555");
	assert(0 == ret);

	int pid = (int)getpid();
	int i;
	/*
	 * send out a number of mp-messages
	 * (as documented in the comment above stress())
	 * all mp-messages have a different prefix.
	 */
	for (i=20; i--;) {
		char id_prefix[21];
		snprintf(id_prefix, 21, "%i.%i.", pid, i);
		if (stress(socket, id_prefix)) {
			fprintf(stderr, "failed to send stress chunk %i\n", i);
			assert(0 == zmq_close(socket));
			assert(0 == zmq_term(context));
			return 5;
		}
	}

	assert(0 == zmq_close(socket));
	assert(0 == zmq_term(context));

	return 0;
}
