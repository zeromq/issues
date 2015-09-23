
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <zmq.h>

#define ENDPOINT                "tcp://127.0.0.1:20000"
#define FIRST_PART              "this"
#define FIRST_PART_LEN          (sizeof(FIRST_PART) - 1)
#define SECOND_PART             "that"
#define SECOND_PART_LEN         (sizeof(SECOND_PART) - 1)
#define SLEEP_TIME              1


int
main() {
    int                         major, minor, patch, ret;
    void                       *ctx, *sender, *receiver;

    zmq_version(&major, &minor, &patch);
    printf("Current ØMQ version is %d.%d.%d\n", major, minor, patch);

    ctx = zmq_ctx_new();
    assert(ctx);

    sender = zmq_socket(ctx, ZMQ_DEALER);
    assert(sender);

    receiver = zmq_socket(ctx, ZMQ_DEALER);
    assert(receiver);

    ret = zmq_bind(sender, ENDPOINT);
    assert(ret == 0);

    ret = zmq_connect(receiver, ENDPOINT);
    assert(ret == 0);

    sleep(SLEEP_TIME);

    //  Send 1-st part of message
    ret = zmq_send(sender, FIRST_PART, FIRST_PART_LEN, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    assert(ret == FIRST_PART_LEN);

    //  Close the receiver
    ret = zmq_close(receiver); 
    assert (ret == 0);
    sleep(SLEEP_TIME);

    //  Tries to send 2-nd part... and possibly get Assertion failed: !more (lb.cpp:97)
    ret = zmq_send(sender, SECOND_PART, SECOND_PART_LEN, ZMQ_DONTWAIT);
    assert(ret == -1);

    zmq_close(sender);
    zmq_ctx_destroy(ctx);

    return 0;
}
