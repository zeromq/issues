//  Test MORE flag on XSUB/XPUB socket

#include "czmq.h"

int main (void)
{
    zctx_t *ctx = zctx_new ();

    void *server = zsocket_new (ctx, ZMQ_XPUB);
    zsocket_bind (server, "tcp://*:6001");

    void *client = zsocket_new (ctx, ZMQ_XSUB);
    zsocket_connect (client, "tcp://localhost:6001");
    byte subscribe [] = { 1 };
    
    zmq_send (client, &subscribe, sizeof (subscribe), 0);
    zmq_send (client, "MOREMOREMORE", 12, ZMQ_SNDMORE);
    zmq_send (client, "LAST", 4, 0);

    //  First message is subscription, 1 byte
    zframe_t *frame = zframe_recv (server);
    assert (zframe_size (frame) == 1);
    assert (zsocket_rcvmore (server) == 0);
    zframe_destroy (&frame);
    
    //  Second message is 12 bytes with MORE
    frame = zframe_recv (server);
    assert (zframe_size (frame) == 12);
    assert (zsocket_rcvmore (server) == 1);
    zframe_destroy (&frame);
    
    //  Third message is 4 bytes with no more
    frame = zframe_recv (server);
    assert (zframe_size (frame) == 4);
    assert (zsocket_rcvmore (server) == 0);
    zframe_destroy (&frame);
    
    zctx_destroy (&ctx);
    return 0;
}
