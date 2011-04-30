#include "zmq.h"
#include "assert.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"

int main ()
{
    void *ctx = zmq_init (1);
    assert (ctx);
    void *s = zmq_socket (ctx, ZMQ_PUB);
    uint64_t sz = 100;
    int rc = zmq_setsockopt (s, ZMQ_HWM, &sz, sizeof (sz));
    assert (rc == 0);
    rc = zmq_bind (s, "tcp://127.0.0.1:2211");
    assert (rc == 0);

    zmq_msg_t msg;
    while (1) {
        puts ("sending");
        zmq_msg_init_size (&msg, 5);
        rc = zmq_send (s, &msg, ZMQ_SNDMORE);
        zmq_msg_close (&msg);

        zmq_msg_init_size (&msg, 4);
        rc = zmq_send (s, &msg, ZMQ_SNDMORE);
        zmq_msg_close (&msg);

        zmq_msg_init_size (&msg, 1000);
        rc = zmq_send (s, &msg, 0);
        zmq_msg_close (&msg);
    }

    return 0;
}