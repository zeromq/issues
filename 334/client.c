#include "czmq.h"

int main (void) {
    zctx_t *ctx = zctx_new ();
    void *client = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_bind (client, "tcp://*:5555");
    
    zmsg_t *msg = zmsg_new();
    zmsg_pushstr (msg, "username");
    zmsg_send    (&msg, client);
    zmsg_destroy (&msg);

    zclock_sleep (1000);
    zctx_destroy (&ctx);
    return 0;
}

