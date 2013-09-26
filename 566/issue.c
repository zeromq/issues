#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "zmq.h"

#define print(...) printf(__VA_ARGS__); fflush(stdout);
 
void _check_rc(int rc) {
    if (rc < 0) {
        printf("%s\n", zmq_strerror(zmq_errno()));
    }
}
 
int main (void) {
    void *ctx1 = zmq_init (1);
    void *ctx2 = zmq_init (1);
        
    void *router = zmq_socket (ctx1, ZMQ_ROUTER);
    int rc = zmq_bind (router, "tcp://127.0.0.1:5555");
    assert (rc != -1);
 
    int attempt;
    print("zeromq-%d.%d\n", ZMQ_VERSION_MAJOR, ZMQ_VERSION_MINOR);
    for (attempt = 0; attempt < 1000; attempt++) {
        print("Attempt %d: ", attempt);
 
        void *dealer = zmq_socket (ctx2, ZMQ_DEALER);
        char ident [10];
        sprintf (ident, "%09d", attempt);
        print ("%s ", ident);
        
        zmq_setsockopt (dealer, ZMQ_IDENTITY, ident, 10);
        zmq_connect (dealer, "tcp://127.0.0.1:5555");
        int rcvtimeo = 1000;
        zmq_setsockopt (dealer, ZMQ_RCVTIMEO, &rcvtimeo, sizeof(int));
        usleep (10000);
        print (".");
        
        rc = zmq_send (router, ident, 10, ZMQ_SNDMORE);
        _check_rc(rc);
        assert (rc == 10);
        print(".");
        
        rc = zmq_send (router, ident, 10, 0);
        _check_rc(rc);
        assert (rc == 10);
        print(". ");
        
        char buf [12];
        rc = zmq_recv(dealer, buf, 10, 0);
        _check_rc(rc);
        assert (rc == 10);
        print(" %s OK\n", buf);
        
        int linger = 0;
        rc = zmq_setsockopt (dealer, ZMQ_LINGER, &linger, sizeof (int));
        _check_rc(rc);
        zmq_close (dealer);
    }
    zmq_ctx_destroy (ctx1);
    zmq_ctx_destroy (ctx2);
    return 0;
}
