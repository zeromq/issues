#include "zmq.h"
#include "assert.h"
#include "stdint.h"

int main ()
{
    void *ctx = zmq_init (1);
    assert (ctx);
    void *s = zmq_socket (ctx, ZMQ_SUB);
    int rc = zmq_setsockopt (s, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0); 
    int sz = 100;
    rc = zmq_setsockopt (s, ZMQ_RCVHWM, &sz, sizeof (sz));
    assert (rc == 0);
    rc = zmq_connect (s, "tcp://127.0.0.1:2211");
    assert (rc == 0);

    char buff [1000];
    while (1) {
        rc = zmq_recv (s, buff, 1000, 0);
        assert (rc == 5);
        int more;
        size_t moresz;
        rc = zmq_getsockopt (s, ZMQ_RCVMORE, &more, &moresz);
        assert (rc == 0);
        assert (more);

        rc = zmq_recv (s, buff, 1000, 0);
        assert (rc == 4);
        rc = zmq_getsockopt (s, ZMQ_RCVMORE, &more, &moresz);
        assert (rc == 0);
        assert (more);

        rc = zmq_recv (s, buff, 1000, 0);
        assert (rc == 1000);
        rc = zmq_getsockopt (s, ZMQ_RCVMORE, &more, &moresz);
        assert (rc == 0);
        assert (!more);
    }

    return 0;
}
