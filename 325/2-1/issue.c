#include "czmq.h"


int main (int argc, char * argv[])
{
    void *ctx, *s1, *s2, *s3;

    ctx = zmq_init(1);
    s1 = zmq_socket(ctx, ZMQ_REP);
    zmq_bind(s1, "tcp://*:5557");

    s2 = zmq_socket(ctx, ZMQ_REQ);
    zmq_setsockopt(s2, ZMQ_IDENTITY, "test", 4);
    zmq_connect(s2, "tcp://localhost:5557");

    s3 = zmq_socket(ctx, ZMQ_REQ);
    zmq_setsockopt(s3, ZMQ_IDENTITY, "test", 4);
    zmq_connect(s3, "tcp://localhost:5557");

    zmq_msg_t msg;
    
    zmq_msg_init (&msg);
    zmq_send (s2, &msg, 0);
    zmq_msg_close (&msg);

    zmq_msg_init (&msg);
    zmq_recv (s1, &msg, 0);
    zmq_msg_close (&msg);

    zmq_msg_init (&msg);
    zmq_send (s3, &msg, 0);
    zmq_msg_close (&msg);

    zmq_msg_init (&msg);
    zmq_recv (s1, &msg, 0);
    zmq_msg_close (&msg);

    int zero = 0;
    zmq_setsockopt (s3, ZMQ_LINGER, &zero, sizeof (int));
    zmq_close(s3);

    zmq_msg_init (&msg);
    zmq_send (s2, &msg, 0);
    zmq_msg_close (&msg);
    
    zmq_msg_init (&msg);
    zmq_recv (s1, &msg, 0);
    zmq_msg_close (&msg);

    zmq_setsockopt (s1, ZMQ_LINGER, &zero, sizeof (int));
    zmq_setsockopt (s2, ZMQ_LINGER, &zero, sizeof (int));
    zmq_close(s1);
    zmq_close(s2);
    zmq_term(ctx);
    return 0;
}

