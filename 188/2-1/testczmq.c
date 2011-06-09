//  Test case for 134
//  Should work over czmq with workaround for issue 134
//
#include "czmq.h"

int main (void)
{
    zctx_t *ctx = zctx_new ();

    void *server = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_bind (server, "tcp://*:8000");

    void *client = zsocket_new (ctx, ZMQ_PULL);
    zsocket_connect (client, "tcp://127.0.0.1:8000");

    sleep (1);
    //  Send two parts to server socket
    zmq_msg_t msg;

    zmq_msg_init_size (&msg, 1);
    int rc = zmq_send (server, &msg, ZMQ_SNDMORE);
    assert (rc == 0);
    zmq_msg_close (&msg);

    zmq_msg_init_size (&msg, 1);
    rc = zmq_send (server, &msg, 0);
    assert (rc == 0);
    zmq_msg_close (&msg);

    //  Recv one part off client socket
    zmq_msg_init (&msg);
    rc = zmq_recv (client, &msg, 0);
    assert (rc == 0);

    zctx_destroy (&ctx);
    return 0;
}
