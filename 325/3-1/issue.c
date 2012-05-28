#include <zmq.h>

int main (int argc, char * argv[])
{
    void *ctx, *s1, *s2, *s3;
    int rc;
    char buf [256];

    ctx = zmq_init(1);
    s1 = zmq_socket(ctx, ZMQ_REP);
    rc = zmq_bind(s1, "tcp://*:5557");

    s2 = zmq_socket(ctx, ZMQ_REQ);
    rc = zmq_setsockopt(s2, ZMQ_IDENTITY, "test", 4);
    rc = zmq_connect(s2, "tcp://localhost:5557");

    s3 = zmq_socket(ctx, ZMQ_REQ);
    rc = zmq_setsockopt(s3, ZMQ_IDENTITY, "test", 4);
    rc = zmq_connect(s3, "tcp://localhost:5557");

    rc = zmq_send (s2, "hi", 2, 0);
    rc = zmq_recv (s1, buf, 256, 0);

    rc = zmq_send (s3, "hi", 2, 0);
    rc = zmq_recv (s1, buf, 256, 0);

    zmq_close(s3);

    rc = zmq_send (s2, "hi", 2, 0);
    rc = zmq_recv (s1, buf, 256, 0);

    zmq_close(s1);
    zmq_close(s2);
    zmq_term(ctx);
    return 0;
}

