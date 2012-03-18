#include "czmq.h"

int main (void) {
    zctx_t *ctx = zctx_new ();
    void *server = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_connect (server, "tcp://localhost:5555");

    zmsg_t *msg = zmsg_recv (server);
    char *szuser = zmsg_popstr (msg);
    puts (szuser);
    free (szuser);
    zmsg_destroy (&msg);
    
    zclock_sleep (1000);
    zctx_destroy (&ctx);
    return 0;
}

