#include "czmq.h"

static void *
create_socket (void *context)
{
    void *s = zmq_socket (context, ZMQ_REP);
    assert (s);
    int rc = zmq_close (s);
    
    printf ("RC=%d errno=%d\n", rc, errno);
    if (rc == -1) {
        puts (strerror (errno));
        assert (errno == ETERM);
    }
    return NULL;
}

int main (void)
{
    void *context = zmq_ctx_new ();
    pthread_t child;
    pthread_create (&child, NULL, create_socket, context);
    //  Wait until started creating socket
    zclock_sleep (1);
    zmq_ctx_destroy (context);
    sleep (1);
    return 0;
}


