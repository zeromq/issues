#include <czmq.h>

int main ()
{
    zmsg_t *msg;
    int rc;

    //  Prepare our context and sockets
    zctx_t *context = zctx_new ();
    
    puts ("Testing ZMQ_ROUTER_MANDATORY:");
    void *frontend = zsocket_new (context, ZMQ_ROUTER);
    zsocket_set_identity (frontend, "front");
    zmq_bind (frontend, "tcp://*:6001");
    zsocket_set_router_mandatory (frontend, 1);

    //  Step 1, backend not ready
    msg = zmsg_new ();
    zmsg_addstr (msg, "back");
    zmsg_addstr (msg, "data");
    rc = zmsg_send (&msg, frontend);
    assert (rc == -1);
    assert (errno == EHOSTUNREACH);
    puts (" -- step 1 OK");

    //  Step 2, backend ready
    void *backend = zsocket_new (context, ZMQ_ROUTER);
    zsocket_set_identity (frontend, "back");
    zmq_connect (backend, "tcp://localhost:6001");
    sleep (1);
    
    msg = zmsg_new ();
    zmsg_addstr (msg, "back");
    zmsg_addstr (msg, "data");
    rc = zmsg_send (&msg, frontend);
    assert (rc == 0);
    puts (" -- step 2 OK");

    //  Step 3, backend not ready
    zmq_close (backend);
    sleep (1);
    msg = zmsg_new ();
    zmsg_addstr (msg, "back");
    zmsg_addstr (msg, "data");
    rc = zmsg_send (&msg, frontend);
    assert (rc == -1);
    assert (errno == EHOSTUNREACH);
    puts (" -- step 3 OK");

    //  Step 4, backend ready again
    zmq_bind (backend, "tcp://*:6001");
    sleep (1);
    msg = zmsg_new ();
    zmsg_addstr (msg, "back");
    zmsg_addstr (msg, "data");
    rc = zmsg_send (&msg, frontend);
    assert (rc == 0);
    puts (" -- step 4 OK");

    zctx_destroy (&context);
    return 0;
}