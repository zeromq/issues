#include "zmq.h"
#include "assert.h"
#include "stdio.h"

int main (void) 
{
    void *context = zmq_init (1);
    void *client = zmq_socket (context, ZMQ_PULL);
    int rc = zmq_connect (client, "tcp://localhost:localhost");
    assert (rc == -1);
    rc = zmq_connect (client, "tcp://invalid:address");
    assert (rc == -1);
    rc = zmq_connect (client, "tcp://imatix.invalid:9000");
    assert (rc == -1);

    int linger = 0;
    zmq_setsockopt (client, ZMQ_LINGER, &linger, sizeof (int));
    zmq_close (client);
    zmq_term (context);
    return 0;
}
