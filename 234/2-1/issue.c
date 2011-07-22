#include "czmq.h"

int main (void)
{
    zctx_t *ctx = zctx_new ();
    
    void *reader = zsocket_new (ctx, ZMQ_DEALER);
    int rc = zmq_bind (reader, "tcp://127.0.0.1:9001");
    assert (rc == 0);

    char *message = zstr_recv (reader);
    if (!message)
        puts ("Interrupted by Ctrl-C");

    zctx_destroy (&ctx);
    return 0;
}

