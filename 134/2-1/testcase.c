//  Test case for 134
//
#include "zmq.h"
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

int main (void)
{
    void *ctx = zmq_init (1);

    void *server = zmq_socket (ctx, ZMQ_PUSH);
    int rc = zmq_bind (server, "tcp://*:8000");
    assert (rc == 0);

    void *client = zmq_socket (ctx, ZMQ_PULL);
    rc = zmq_connect (client, "tcp://127.0.0.1:8000");
    assert (rc == 0);

    sleep (1);
    //  Send two parts to server socket
    zmq_msg_t msg;

    zmq_msg_init_size (&msg, 1);
    rc = zmq_send (server, &msg, ZMQ_SNDMORE);
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

    //  Close sockets and terminate
    zmq_close (server);
    zmq_close (client);
    zmq_term (ctx);
    return 0;
}
