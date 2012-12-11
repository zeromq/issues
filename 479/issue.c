//
//  This example shows how to detect when a peer is available or
//  offline, using the ZMQ_ROUTER_MANDATORY socket option. It
//  works for the ROUTER-DEALER pattern.
//
#include "czmq.h"

int main (void)
{
    zctx_t *ctx = zctx_new ();
    void *router = zsocket_new (ctx, ZMQ_ROUTER);
    zsocket_set_router_mandatory (router, true);
    int port = zsocket_bind (router, "tcp://*:*");

    //  Set-up initial connection, check that works
    void *dealer = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_connect (dealer, "tcp://localhost:%d", port);
    zstr_send (dealer, "HELLO");
    zmsg_t *msg = zmsg_recv (router);
    assert (zmsg_size (msg) == 2);
    zmsg_dump (msg);

    zsocket_disconnect (dealer, "tcp://localhost:%d", port);
    zclock_sleep (500);
    int rc = zmsg_send (&msg, router);
    //  This should return -1
    assert (rc == 0);
    
    zctx_destroy (&ctx);
    return 0;
}
