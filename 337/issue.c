#include "zmq.h"

int main (void) {
    void *context = zmq_init (1);
    void *client = zmq_socket (context, ZMQ_DEALER);
    zmq_bind (client, "tcp://*:5555");
    
    void *server = zmq_socket (context, ZMQ_DEALER);
    zmq_connect (server, "tcp://localhost:5555");

    zmq_msg_t msg;
    zmq_msg_init  (&msg);
    zmq_msg_send  (&msg, client, 0);
    zmq_msg_close (&msg);

    zmq_msg_init  (&msg);
    zmq_msg_recv  (&msg, server, 0);
    zmq_msg_close (&msg);

    zmq_close (client);
    zmq_close (server);
    zmq_term (context);
    return 0;
}
