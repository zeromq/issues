#include "czmq.h"

int main (int argc, char *argv [])
{
    void *context = zmq_init (1);
    void *pusher = zmq_socket (context, ZMQ_PUSH);

    zmq_bind (pusher, "tcp://127.0.0.1:5858");
    
    //  Connect normal socket to 0MQ PUSH socket
    int puller = socket (AF_INET, SOCK_STREAM, 0);
    assert (puller >= 0);
    
    struct sockaddr_in addr;
    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
    addr.sin_port = htons (5858);
    int rc = connect (puller, (struct sockaddr *) &addr, sizeof (addr));
    assert (rc == 0);
    
    //  Send ZMTP greeting message
    unsigned char buf [512];
    buf [0] = 0x01;
    buf [1] = 0x00;
    rc = send (puller, buf, 2, 0);
    assert (rc == 2);

    //  Send a message just to ensure there's something to read
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    zmq_send (pusher, &msg, 0);
    zmq_msg_close (&msg);

    //  Read raw ZMTP frame from normal socket
    rc = recv (puller, buf, 512, 0);
    assert (rc >= 2);

    //  Small or large frame?
    int flags_off = 1;
    if (buf [0] == 255) {
        assert (rc >= 10);
        flags_off = 9;
    }
    //  Check reserved flags are set to zero
    assert (buf [flags_off] == 0);

    zmq_close (pusher);
    zmq_term (context);

    return 0;
}
