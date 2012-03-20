#include "zmq.h"
#include "assert.h"

int main (void) {
    void *context = zmq_ctx_new ();
    zmq_ctx_set (context, ZMQ_MAX_SOCKETS, 5);
    int max_sockets = zmq_ctx_get (context, ZMQ_MAX_SOCKETS);
    assert (max_sockets == 5);

    void *s1 = zmq_socket (context, ZMQ_DEALER);
    assert (s1);
    void *s2 = zmq_socket (context, ZMQ_DEALER);
    assert (s2);
    void *s3 = zmq_socket (context, ZMQ_DEALER);
    assert (s3);
    void *s4 = zmq_socket (context, ZMQ_DEALER);
    assert (s4);
    void *s5 = zmq_socket (context, ZMQ_DEALER);
    assert (s5);
    void *s6 = zmq_socket (context, ZMQ_DEALER);
    assert (s6 == NULL);
    
    zmq_close (s1);
    zmq_close (s2);
    zmq_close (s3);
    zmq_close (s4);
    zmq_close (s5);

    zmq_ctx_destroy (context);
    return 0;
}
