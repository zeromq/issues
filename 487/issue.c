//
#include <czmq.h>

static void
listener_task (void *args, zctx_t *ctx, void *pipe)
{
    void *listener = zsocket_new (ctx, ZMQ_SUB);
    while (true) {
        puts ("Connecting...");
        int rc = zsocket_connect (listener, "inproc://me");
        if (rc == 0) {
            puts ("Connect successful");
            break;
        }
        puts (strerror (errno));
        sleep (1);
    }
    zstr_send (listener, "Done");
}

int main (int argc, char *argv [])
{
    zctx_t *ctx = zctx_new ();
    zthread_fork (ctx, listener_task, argv [1]);

    void *broadcaster = zsocket_new (ctx, ZMQ_PUB);
    sleep (1);
    puts ("Binding...");
    zsocket_bind (broadcaster, "inproc://me");
    char *message = zstr_recv (broadcaster);
    free (message);
    zctx_destroy (&ctx);
    return 0;
}
