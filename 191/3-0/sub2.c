#include "zmq.h"
#include "assert.h"
#include "stdint.h"
#include "stdio.h"

int
zsockopt_rcvmore (void *socket)
{
    int rcvmore;
    size_t type_size = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RCVMORE, &rcvmore, &type_size);
    return rcvmore;
}


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

    while (1) {
        puts ("receiving");
        zmq_msg_t msg;
        zmq_msg_init (&msg);
        rc = zmq_recvmsg (s, &msg, 0);
        assert (zmq_msg_size (&msg) == 5);
        zmq_msg_close (&msg);
        int more = zsockopt_rcvmore (s);
        assert (more);

        zmq_msg_init (&msg);
        rc = zmq_recvmsg (s, &msg, 0);
        assert (zmq_msg_size (&msg) == 4);
        zmq_msg_close (&msg);
        more = zsockopt_rcvmore (s);
        assert (more);

        zmq_msg_init (&msg);
        rc = zmq_recvmsg (s, &msg, 0);
        assert (zmq_msg_size (&msg) == 1000);
        zmq_msg_close (&msg);
        more = zsockopt_rcvmore (s);
        assert (!more);
    }
    return 0;
}

