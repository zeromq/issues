#include "czmq.h"

//  Should be added to zsocket
//  Returns TRUE if socket polled true on input, else false
static Bool zsocket_poll (void *socket, int timeout)
{
    zmq_pollitem_t items [] = { { socket, 0, ZMQ_POLLIN, 0 } };
    int rc = zmq_poll (items, 1, timeout);
    return (items [0].revents & ZMQ_POLLIN) != 0;
}


int main() {
    zctx_t *context = zctx_new ();
    
    void *router = zsocket_new (context, ZMQ_ROUTER);
    zmq_bind (router, "tcp://*:6666");

    void *dealer = zsocket_new (context, ZMQ_DEALER);
    zmq_connect (dealer, "tcp://127.0.0.1:6666");
    zclock_sleep (250);

    puts ("DEALER sending ping");
    zstr_send (dealer, "ping");
    
    zmsg_t *msg = zmsg_recv (router);
    puts ("ROUTER received"); zmsg_dump (msg);
    puts ("ROUTER sending"); zmsg_dump (msg);
    zmsg_send (&msg, router);
    
    msg = zmsg_recv (dealer);
    puts ("DEALER received");
    zmsg_dump (msg);

    puts ("Restarting router...");
    zsocket_destroy (context, router);
    zclock_sleep (250);
    router = zsocket_new (context, ZMQ_ROUTER);
    zmq_bind (router, "tcp://*:6666");

    //  Dealer gets inappropriate empty message on rebind
    if (zsocket_poll (dealer, 100)) {
        msg = zmsg_recv (dealer);
        puts ("!!! DEALER received spurious message"); zmsg_dump (msg);
        zmsg_destroy (&msg);
    }
    puts ("DEALER sending ping2");
    zstr_send (dealer, "ping2");

    if (!zsocket_poll (router, 100)) 
        puts ("!!!! ROUTER didn't receive ping2");

    puts ("DEALER sending ping3");
    zstr_send (dealer, "ping3");
        
    if (!zsocket_poll (router, 100))
        puts ("ROUTER didn't receive ping3");

    msg = zmsg_recv (router);
    puts ("ROUTER received"); zmsg_dump (msg);
    puts ("ROUTER sending"); zmsg_dump (msg);
    zmsg_send (&msg, router);

    msg = zmsg_recv (dealer);
    puts ("DEALER received"); zmsg_dump (msg);
    zmsg_destroy (&msg);

    zctx_destroy (&context);
    return 0;
}

