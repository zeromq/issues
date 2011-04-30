#include "zmq.h"
#include "assert.h"
#include "stdint.h"
#include "string.h"

int main ()
{
    void *ctx = zmq_init (1);
    assert (ctx);
    void *s = zmq_socket (ctx, ZMQ_PUB);
    int sz = 100;
    int rc = zmq_setsockopt (s, ZMQ_SNDHWM, &sz, sizeof (sz));
    assert (rc == 0);
    rc = zmq_bind (s, "tcp://127.0.0.1:2211");
    assert (rc == 0);

    uint32_t seq = 0;
    char buff [1000];
    memset (buff, 111, 1000);
    while (1) {
        rc = zmq_send (s, "topic", 5, ZMQ_SNDMORE);
        assert (rc == 5);
        rc = zmq_send (s, &seq, 4, ZMQ_SNDMORE);
        assert (rc == 4);
        rc = zmq_send (s, buff, 1000, 0);
        assert (rc == 1000);
    }

    return 0;
}
