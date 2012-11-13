#include "czmq.h"

int main () {
    zctx_t *context = zctx_new ();
    
    puts ("Testing socket destruction:");
    void *router = NULL; 
    void *dealer = NULL;
    
    while (!zctx_interrupted) {
        //  Randomly bind or unbind router
        if (randof (10) == 0 && !router) {
            router = zsocket_new (context, ZMQ_ROUTER);
            zmq_bind (router, "tcp://*:6666");
            printf ("*"); fflush (stdout);
        }
        else
        if (randof (10) == 0 && router) {
            zsocket_destroy (context, router);
            router = NULL;
            printf ("."); fflush (stdout);
        }
        //  Randomly connect and disconnect socket
        if (randof (10) == 0 && !dealer) {
            dealer = zsocket_new (context, ZMQ_DEALER);
            zmq_connect (dealer, "tcp://localhost:6666");
            printf ("#"); fflush (stdout);
        }
        else
        if (randof (10) == 0 && dealer) {
            zsocket_destroy (context, dealer);
            dealer = NULL;
            printf ("-"); fflush (stdout);
        }
        //  Send message out via DEALER
        if (dealer) {
            zstr_sendm (dealer, "frame 1");
            zstr_send  (dealer, "frame 2");
        }
        //  Read any messages waiting on ROUTER
        if (router) {
            zmq_pollitem_t pollitems [] = { { router, 0, ZMQ_POLLIN, 0 } };
            if (zmq_poll (pollitems, 1, randof (100) * ZMQ_POLL_MSEC) == -1) {
                puts (strerror (errno));
                break;              //  Interrupted
            }
            if (pollitems [0].revents & ZMQ_POLLIN) {
                zmsg_t *msg = zmsg_recv (router);
                zmsg_destroy (&msg);
            }
        }
        printf ("."); fflush (stdout);
    }
    zctx_destroy (&context);
    return 0;
}
