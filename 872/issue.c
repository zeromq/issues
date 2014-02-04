#include <czmq.h>

int main (void)
{
    void *ctx = zmq_ctx_new ();
    void *sender = zmq_socket (ctx, ZMQ_DEALER);
    zmq_connect(sender, "inproc://aa");

    //  If first part is empty, receiver gets identity frame instead
    //  else does not get identity frame, but does get first part.
    zmq_send (sender, "a", 0, ZMQ_SNDMORE);
    zmq_send (sender, "b", 1, ZMQ_SNDMORE);
    zmq_send (sender, "123", 3, 0);

    void *receiver = zmq_socket (ctx, ZMQ_ROUTER);
    zmq_bind (receiver, "inproc://aa");

    zmsg_t *msg = zmsg_recv (receiver);
    zmsg_dump (msg);
    assert (zmsg_size (msg) == 4);
    zmsg_destroy (&msg);

    return 0;
}
