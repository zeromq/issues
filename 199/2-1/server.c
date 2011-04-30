#include <zmq.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>


#include <sys/types.h>
#include <unistd.h>

void to_hex(const char *data, size_t len, char *dest) {
	int i;
	for (i=0; i<len; ++i) {
		snprintf(dest+2*i, 3, "%02x", (int)data[i]);
	}
}

void stupid_recv(void *zsock) {
	int64_t more;
	size_t more_size = sizeof (more);

	assert(zsock);

	zmq_msg_t message;
	assert(0 == zmq_msg_init(&message));

	assert( 0 == zmq_recv(zsock, &message, 0) );
	assert(zmq_msg_size(&message) > 0);
	assert( 0 == zmq_getsockopt(zsock, ZMQ_RCVMORE, &more, &more_size) );

	// check message size
	fprintf(stderr, "STUPID RECV (JUST FOR SHOWING WHATS WAITING): mp part [%.*s] (%p) received. more==%li.\n",
				(int)zmq_msg_size(&message), (char*)zmq_msg_data(&message),
				zmq_msg_data(&message), more);
	fflush(stderr);

	assert( 0 == zmq_msg_close(&message) );
}

// * receive a message from socket
// * message contents must equal `stress_id`+`expectation`
// * check if ZMQ_RCVMORE is set, must match parameter `expect_more`
void recv_assert(void *zsock, const char *identity,
		const char *stress_id, const char *expectation, char expect_more) {
	int64_t more;
	size_t more_size = sizeof (more);

	assert(zsock);

	zmq_msg_t message;
	assert(0 == zmq_msg_init(&message));

	assert( 0 == zmq_recv(zsock, &message, 0) );
	assert(zmq_msg_size(&message) > 0);
	assert( 0 == zmq_getsockopt(zsock, ZMQ_RCVMORE, &more, &more_size) );

	// check message size
	if ((strlen(stress_id) + strlen(expectation)) != zmq_msg_size(&message)) {
		fprintf(stderr, "%s: mp part [%.*s] (%p) but expected [%s%s] (size missmatch).\n",
				identity, (int)zmq_msg_size(&message), (char*)zmq_msg_data(&message),
				zmq_msg_data(&message),
				stress_id, expectation); fflush(stderr);
		while (1) stupid_recv(zsock);
		assert(0);
	}

	// check message contents
	size_t stress_id_len = strlen(stress_id);
	if (strncmp(stress_id, (char*)zmq_msg_data(&message), stress_id_len) ||
		strncmp(expectation, ((char*)zmq_msg_data(&message))+stress_id_len, strlen(expectation))) {
		fprintf(stderr, "%s: mp part [%.*s] (%p) but expected [%s%s].\n",
				identity, (int)zmq_msg_size(&message), (char*)zmq_msg_data(&message),
				zmq_msg_data(&message),
				stress_id, expectation); fflush(stderr);
		while (1) stupid_recv(zsock);
		assert(0);
	}

	// check if we expect more, but there is no more
	if (expect_more && (!more)) {
		fprintf(stderr, "%s: mp part [%.*s] ZMQ_RCVMORE not set (unexpected!)\n",
				identity, (int)zmq_msg_size(&message), (char*)zmq_msg_data(&message)); fflush(stderr);
		while (1) stupid_recv(zsock);
		assert(0);
	}

	// check if we expect last message, but there are more
	if ((!expect_more) && more) {
		fprintf(stderr, "%s: mp part [%.*s] ZMQ_RCVMORE set (unexpected!)\n",
				identity, (int)zmq_msg_size(&message), (char*)zmq_msg_data(&message)); fflush(stderr);
		while (1) stupid_recv(zsock);
		assert(0);
	}

	// we got what we expected - dont assert(0)
	fprintf(stderr, "%s: mp part [%.*s]", identity, (int)zmq_msg_size(&message), (char*)zmq_msg_data(&message));
	if (expect_more) {
		fprintf(stderr, " and more.");
	} else {
		fprintf(stderr, " and no more.");
	}
	fprintf(stderr, " (as expected)\n"); fflush(stderr);
	assert( 0 == zmq_msg_close(&message) );
}

void receive_mp(void *zsock) {
	int64_t more;
	size_t more_size = sizeof (more);

	assert(zsock);

	zmq_msg_t id_message;
	assert(0 == zmq_msg_init(&id_message));

	assert( 0 == zmq_recv(zsock, &id_message, 0) );
	assert( 0 == zmq_getsockopt(zsock, ZMQ_RCVMORE, &more, &more_size) );

	assert(more);
	// we store the identity
	assert(zmq_msg_size(&id_message) > 0);
	char *identity = (char*)malloc(zmq_msg_size(&id_message)*2+1);
	assert(identity);
	to_hex((char*)zmq_msg_data(&id_message), zmq_msg_size(&id_message), identity);
	assert( 0 == zmq_msg_close(&id_message) );
	
	// receive first message
	// this message starts with a number which is the
	// prefix for all following message
	// (to detect intermixed mp-messages)
	zmq_msg_t first_message;
	assert(0 == zmq_msg_init(&first_message));
	more_size = sizeof (more);
	assert( 0 == zmq_recv(zsock, &first_message, 0) );
	assert( 0 == zmq_getsockopt(zsock, ZMQ_RCVMORE, &more, &more_size) );
	assert(more);

	assert(zmq_msg_data(&first_message));
	// find length of this "prefix"
	int stress_id_len = strspn((char*)zmq_msg_data(&first_message), "0123456789.");
	assert(stress_id_len > 0);
	char *stress_id = (char*)malloc(stress_id_len+1);
	assert(stress_id);
	memcpy(stress_id, zmq_msg_data(&first_message), stress_id_len);
	stress_id[stress_id_len] = 0;

	// check if the first message has the form PREFIX+"stress"
	if (0 == strncmp((char*)zmq_msg_data(&first_message) + stress_id_len, "stress", 6)) {
		fprintf(stderr, "%s: incoming stress mp [%.*s]\n", identity, 
				(int)zmq_msg_size(&first_message), (char*)zmq_msg_data(&first_message)); fflush(stderr);
	} else {
		// protocol violation (e.g. thats not what we sent)
		fprintf(stderr, "%s: mp does not start with \"XXXXstress\": [%.*s] (%p)\n",
				identity, (int)zmq_msg_size(&first_message), (char*)zmq_msg_data(&first_message),
				zmq_msg_data(&first_message)); fflush(stderr);
		while (1) stupid_recv(zsock);
		assert(0);
	}
	assert( 0 == zmq_msg_close(&first_message) );

	recv_assert(zsock, identity, stress_id, "STRESS A", 1);
	recv_assert(zsock, identity, stress_id, "STRESS B", 1);
	recv_assert(zsock, identity, stress_id, "STRESS C", 1);
	recv_assert(zsock, identity, stress_id, "STRESS D", 1);
	recv_assert(zsock, identity, stress_id, "STRESS E", 0);
	free(stress_id);
	free(identity);
}

int main(int argc, char **argv)
{
	// create context
	void *context = zmq_init(1);
	assert(context);

	// create socket
	void *zsock = zmq_socket(context, ZMQ_XREP);
	assert(zsock);
	
	// listen
	int ret = zmq_bind(zsock, "tcp://127.0.0.1:5555");
	assert(0 == ret);

	// wait until protocol is violated...
	while (1) receive_mp(zsock);
	//while (1) stupid_recv(zsock);

	// this wont be reached...
	assert(0 == zmq_close(zsock));
	assert(0 == zmq_term(context));
	return 0;
}
