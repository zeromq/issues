#include <czmq.h>

int main (void)
{
    zctx_t *ctx=zctx_new();
    void *router = zsocket_new (ctx, ZMQ_ROUTER);
    void *dealer = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_set_identity (dealer, "id");

    int rc = zsocket_bind (router,"tcp://*:51000");
    assert (rc == 51000);

    puts ("ping-pong 1");
    rc = zsocket_connect (dealer, "tcp://127.0.0.1:51000");
    assert (rc == 0);
    zmsg_t *msg;
    zstr_send (dealer, "HELLO");
    msg = zmsg_recv (router);
    zmsg_dump (msg);
    zmsg_send (&msg, router);
    msg = zmsg_recv (dealer);
    zmsg_dump (msg);

    puts ("ping-pong 2");
    zsocket_destroy (ctx, dealer);
    dealer = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_set_identity (dealer, "id");
    rc = zsocket_connect (dealer, "tcp://127.0.0.1:51000");
    assert (rc == 0);
    zstr_send (dealer, "WORLD");
    msg = zmsg_recv (router);
    zmsg_dump (msg);
    zmsg_send (&msg, router);
    msg = zmsg_recv (dealer);
    zmsg_dump (msg);
    
    puts ("ping-pong 3");
    rc = zsocket_disconnect (dealer, "tcp://127.0.0.1:51000");
    assert (rc == 0);
    rc = zsocket_connect (dealer, "tcp://127.0.0.1:51000");
    assert (rc == 0);
    zstr_send (dealer, "WORLD");
    msg = zmsg_recv (router);
    zmsg_dump (msg);
    zmsg_send (&msg, router);
    msg = zmsg_recv (dealer);
    zmsg_dump (msg);

    return 0;
}