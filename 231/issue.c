#include "zmq.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "czmq.h"

int main (int argc, char const* argv[])
{
    void *ctx = zmq_init (1);
    assert (ctx);

    void *host = zmq_socket (ctx, ZMQ_DEALER);
    assert (host);
    
    int rc = zmq_bind (host, "inproc://host");
    assert (rc == 0);
    
    int i;
    for (i = 0; i < 500; i++) {
        void *client = zmq_socket (ctx, ZMQ_DEALER);
        if (!client)
            puts (strerror (errno));
        assert (client);
        int rc;
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        rc = zmq_connect (client, "inproc://host");
        assert (rc == 0);
        zmq_close (client);
        printf ("%d ", i); fflush (stdout);
    }
    return 0;
}
