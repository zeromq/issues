//  To provoke error, run this, suspend latop, then resume
//  Does not happen systematically though...

#include <czmq.h>

static void
listener_thread (void *args, zctx_t *ctx, void *pipe) 
{
    void *listener = zsocket_new (ctx, ZMQ_SUB);
    zsocket_set_subscribe (listener, "");
    
    int address;
    for (address = 1; address < 255; address++) 
        zsocket_connect (listener, "tcp://192.168.69.%d:9000", address);
    
    while (!zctx_interrupted) {
        char *message = zstr_recv (listener);
        if (message) {
            puts (message);
            free (message);
        }
    }
}

int main (void)
{
    zctx_t *ctx = zctx_new ();
    void *broadcaster = zsocket_new (ctx, ZMQ_PUB);

    zthread_fork (ctx, listener_thread, NULL);
    zsocket_bind (broadcaster, "tcp://*:9000");
    while (!zctx_interrupted) {
        char message [1024];
        if (!fgets (message, 1024, stdin))
            break;
        message [strlen (message) - 1] = 0;
        zstr_send (broadcaster, "pieter: %s", message);
    }
    zctx_destroy (&ctx);
    return 0;
}
