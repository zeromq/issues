#include <czmq.h>

//  Send 1-byte messages via different routes to test batching

#define MSG_COUNT 1000000

void
s_pusher (void *args, zctx_t *ctx, void *pipe)
{
    void *push = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_bind (push, "tcp://127.0.0.1:9000");

    int msg_nbr;
    for (msg_nbr = 0; msg_nbr < MSG_COUNT; msg_nbr++)
        zmq_send (push, "A", 1, 0);
    zmq_send (push, "Z", 1, 0);
    zstr_send (pipe, "done");
}

void
s_puller (void *args, zctx_t *ctx, void *pipe)
{
    void *pull = zsocket_new (ctx, ZMQ_PULL);
    zsocket_connect (pull, "tcp://127.0.0.1:%d", *(int *) args);

    while (true) {
        char message;
        zmq_recv (pull, &message, 1, 0);
        if (message == 'Z')
            break;
    }
    zstr_send (pipe, "done");
}

void
s_proxy_simple (void *args, zctx_t *ctx, void *pipe)
{
    void *pull = zsocket_new (ctx, ZMQ_PULL);
    void *push = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_connect (pull, "tcp://127.0.0.1:9000");
    zsocket_bind (push, "tcp://127.0.0.1:9001");

    while (true) {
        char message;
        zmq_recv (pull, &message, 1, 0);
        zmq_send (push, &message, 1, 0);
        if (message == 'Z')
            break;
    }
    zstr_send (pipe, "done");
}

void
s_proxy_polling (void *args, zctx_t *ctx, void *pipe)
{
    void *pull = zsocket_new (ctx, ZMQ_PULL);
    void *push = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_connect (pull, "tcp://127.0.0.1:9000");
    zsocket_bind (push, "tcp://127.0.0.1:9001");
    zpoller_t *poller = zpoller_new (pull, NULL);

    while (true) {
        char message;
        void *which = zpoller_wait (poller, 1000);
        if (which == pull) {
            zmq_recv (pull, &message, 1, 0);
            zmq_send (push, &message, 1, 0);
            if (message == 'Z')
                break;
        }
        else
            break;
    }
    zstr_send (pipe, "done");
}


int main (void)
{
    void *pusher, *proxy, *puller;
    int64_t start, duration;
    int port_nbr;
    zctx_t *ctx;

    //  Test straight push-to-pull without polling
    printf ("Starting straight push-to-pull test... ");
    fflush (stdout);
    ctx = zctx_new ();
    port_nbr = 9000;
    pusher = zthread_fork (ctx, s_pusher, NULL);
    puller = zthread_fork (ctx, s_puller, &port_nbr);
    zclock_sleep (100);
    start = zclock_time ();
    free (zstr_recv (pusher));
    free (zstr_recv (puller));
    duration = zclock_time () - start;
    printf ("%ld msec\n", (long) duration);
    zctx_destroy (&ctx);

    //  Test simple proxied push-to-pull without polling
    printf ("Starting simple proxied push-to-pull test... ");
    fflush (stdout);
    ctx = zctx_new ();
    port_nbr = 9001;
    pusher = zthread_fork (ctx, s_pusher, NULL);
    proxy = zthread_fork (ctx, s_proxy_simple, NULL);
    puller = zthread_fork (ctx, s_puller, &port_nbr);
    zclock_sleep (100);
    start = zclock_time ();
    free (zstr_recv (puller));
    free (zstr_recv (proxy));
    free (zstr_recv (pusher));
    duration = zclock_time () - start;
    printf ("%ld msec\n", (long) duration);
    zctx_destroy (&ctx);

    //  Test polling proxied push-to-pull without polling
    printf ("Starting polling proxied push-to-pull test... ");
    fflush (stdout);
    ctx = zctx_new ();
    port_nbr = 9001;
    pusher = zthread_fork (ctx, s_pusher, NULL);
    proxy = zthread_fork (ctx, s_proxy_polling, NULL);
    puller = zthread_fork (ctx, s_puller, &port_nbr);
    zclock_sleep (100);
    start = zclock_time ();
    free (zstr_recv (puller));
    free (zstr_recv (proxy));
    free (zstr_recv (pusher));
    duration = zclock_time () - start;
    printf ("%ld msec\n", (long) duration);
    zctx_destroy (&ctx);

    return 0;
}
