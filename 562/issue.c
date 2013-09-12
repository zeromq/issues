#include <czmq.h>

void *client_task (void *args)
{
    zctx_t *ctx = zctx_new ();
    void *subscriber = zsocket_new (ctx, ZMQ_SUB);
    void *pusher = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_connect (subscriber, "tcp://localhost:5563");
    zsocket_connect (pusher, "tcp://localhost:5564");

    int i;
    for (i = 0; i < 1000000; i++) {
        zsocket_set_subscribe (subscriber, "A");

        zmq_pollitem_t polla [] = { { pusher, 0, ZMQ_POLLOUT, 0 } };
        if (zmq_poll (polla, 1, 100) == 1)
            zstr_send (pusher, "B");

        zmq_pollitem_t pollb [] = { { subscriber, 0, ZMQ_POLLIN, 0 } };
        if (zmq_poll (pollb, 1, 100) == 1)
            free (zstr_recv (subscriber));

        zsocket_set_unsubscribe (subscriber, "A");
    }
    return NULL;
}


void *server_task (void *args)
{
    zctx_t *ctx = zctx_new ();
    void *publisher = zsocket_new (ctx, ZMQ_PUB);
    void *puller = zsocket_new (ctx, ZMQ_PULL);
    zsocket_bind (publisher, "tcp://*:5563");
    zsocket_bind (puller, "tcp://*:5564");

    while (true) {
        char *message = zstr_recv (puller);
        if (!message)
            break;
        zstr_sendm (publisher, "A");
        zstr_send  (publisher, "1");
    }
    return NULL;
}

int main (void)
{
    zthread_new (server_task, NULL);
    sleep (1);
    zthread_new (client_task, NULL);
    sleep (1);
    zthread_new (client_task, NULL);
    sleep (10);
    return 0;
}
