#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
s_send(void *socket, char *string) {

#if ZMQ_VERSION_MAJOR == 2

	int rc;
	zmq_msg_t message;
	zmq_msg_init_size(&message, strlen(string));
	memcpy(zmq_msg_data(&message), string, strlen(string));
	rc = zmq_send(socket, &message, 0);
	zmq_msg_close(&message);
	return (rc);

#elif ZMQ_VERSION_MAJOR == 3

	return (zmq_send(socket, string, strlen(string), 0));

#else
# error "unknown ZeroMQ major version"
#endif

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

	print_version("publisher");

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

	socket = zmq_socket(context, ZMQ_PUB);
	if (!socket) {
		perror("zmq_socket");
		return (EXIT_FAILURE);
	}

	fprintf(stderr, "%s: connecting to socket %s\n", argv[0], str_socket);

	if (zmq_connect(socket, str_socket)) {
		perror("zmq_connect");
		return (EXIT_FAILURE);
	}

	for (;;) {
		char buf[512];

		sprintf(buf, "hello %0x", (int) random());
		s_send(socket, buf);
		//printf("sent message: '%s'\n", buf);
		sleep(1);
	}

	zmq_close(socket);
	zmq_term(context);

	return (EXIT_SUCCESS);
}
