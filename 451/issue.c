#include "czmq.h"

int main () {
    zctx_t *context = zctx_new ();
    
    puts ("Testing ZMQ_ROUTER_MANDATORY:");
    void *router = zsocket_new (context, ZMQ_ROUTER);
    zmq_bind (router, "tcp://*:6666");

    //  Test non-mandatory, it will fail silently
    zsocket_set_router_mandatory (router, 0);
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, "identity");
    zmsg_addstr (msg, "data");
    int rc = zmsg_send (&msg, router);
    if (rc == 0)
        puts (" -- step 1 OK");
    else {
        puts (" -- step 1 FAILED");
        return 0;
    }

    //  Test mandatory, it should return EHOSTUNREACH
    zsocket_set_router_mandatory (router, 1);
    msg = zmsg_new ();
    zmsg_addstr (msg, "identity");
    zmsg_addstr (msg, "data");
    rc = zmsg_send (&msg, router);
    if (rc == -1 && errno == EHOSTUNREACH)
        puts (" -- step 2 OK");
    else
    if (rc == -1 && errno == EAGAIN)
        puts (" -- step 2 old behavior");
    else
        puts (" -- step 2 FAILED");

    zctx_destroy (&context);
    return 0;
}
