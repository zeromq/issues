#include <zmq.h>
#include <assert.h>

int main (int argc, char * argv[])
{
    void *ctx, *s1;
    int rc;

    ctx = zmq_init (1);
    s1 = zmq_socket (ctx, ZMQ_REP);
    rc = zmq_bind (s1, "tcp://*:5557");
    zmq_close (s1);

    rc = zmq_recv (s1, "hello", 5, 0);
    assert (rc == -1);
    
    zmq_term(ctx);
    return 0;
}

