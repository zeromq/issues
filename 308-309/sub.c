#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *
s_recv(void *socket) {
	zmq_msg_t message;
	zmq_msg_init(&message);
#if ZMQ_VERSION_MAJOR == 2
	if (zmq_recv(socket, &message, 0) != 0) {
		perror("zmq_recv");
		return (NULL);
	}
#elif ZMQ_VERSION_MAJOR == 3
	if (zmq_recvmsg(socket, &message, 0) == -1) {
		perror("zmq_recvmsg");
		return (NULL);
	}
#else
# error "unsupported ZeroMQ major version"
#endif
	int size = zmq_msg_size(&message);
	char *string = malloc(size + 1);
	memcpy(string, zmq_msg_data(&message), size);
	zmq_msg_close(&message);
	string[size] = 0;
	return (string);
}

void
print_version(const char *s) {
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	printf("%s: zeromq version %d.%d.%d\n", s, major, minor, patch);
}

int main(int argc, char **argv) {
	const char *str_socket;
	void *context, *socket;

	print_version("subscriber");

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <SOCKET>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	str_socket = argv[1];
	
	context = zmq_init(1);
	if (!context) {
		perror("zmq_init");
		return (EXIT_FAILURE);
	}

	socket = zmq_socket(context, ZMQ_SUB);
	if (!socket) {
		perror("zmq_socket");
		return (EXIT_FAILURE);
	}

	fprintf(stderr, "%s: binding to socket %s\n", argv[0], str_socket);

	if (zmq_bind(socket, str_socket)) {
		perror("zmq_bind");
		return (EXIT_FAILURE);
	}

	if (zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0)) {
		perror("zmq_setsockopt");
		return (EXIT_FAILURE);
	}

	for (;;) {
		char *string;

		string = s_recv(socket);
		printf("received message: '%s'\n", string);
		free(string);
	}

	zmq_close(socket);
	zmq_term(context);

	return (EXIT_SUCCESS);
}
