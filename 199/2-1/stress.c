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

	if (zmq_send(socket, &m, more?ZMQ_SNDMORE:0)) {
		fprintf(stderr, "failed to send. errno: %i\n", zmq_errno());
		return 1;
	}
	assert(0 == zmq_msg_close(&m));
	return 0;
}

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

	memcpy(buff+plen, "STRESS A", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send STRESS A\n");
		return 1;
	}

	memcpy(buff+plen, "STRESS B", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send STRESS B\n");
		return 1;
	}

	memcpy(buff+plen, "STRESS C", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send STRESS C\n");
		return 1;
	}

	memcpy(buff+plen, "STRESS D", 8);
	if (zsend(s, buff, plen+8, 1)) {
		fprintf(stderr, "failed to send STRESS D\n");
		return 1;
	}

	memcpy(buff+plen, "STRESS E", 8);
	if (zsend(s, buff, plen+8, 0)) {
		fprintf(stderr, "failed to send STRESS E\n");
		return 1;
	}

	return 0;
}


int main(int argc, char **argv)
{
	if (argc != 1) return 1;

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
