#include "zmq.h"
#include "assert.h"
#include "stdint.h"
#include "stdio.h"
#include "unistd.h"

int main ()
{
    void *ctx = zmq_init (1);
    assert (ctx);
    
    void *pub = zmq_socket (ctx, ZMQ_PUB);
    assert (pub);
    int rc = zmq_bind (pub, "tcp://*:2211");
    assert (rc == 0);
    
    void *sub = zmq_socket (ctx, ZMQ_SUB);
    assert (sub);

    uint64_t size = 1;
    rc = zmq_setsockopt (sub, ZMQ_HWM, &size, sizeof (size));
    assert (rc == 0);
    
    int64_t swap = 10000000;
    rc = zmq_setsockopt (sub, ZMQ_SWAP, &swap, sizeof (swap));
    assert (rc == 0);
    
    rc = zmq_setsockopt (sub, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);
    rc = zmq_connect (sub, "tcp://127.0.0.1:2211");
    assert (rc == 0);

    int count;
    for (count = 0; count < 100000; count++) {
        zmq_msg_t msg;
        zmq_msg_init_size (&msg, 0);
        zmq_send (pub, &msg, 0);
        zmq_msg_close (&msg);
    }
    int linger = 0;
    zmq_setsockopt (pub, ZMQ_LINGER, &linger, sizeof (int));
    zmq_setsockopt (sub, ZMQ_LINGER, &linger, sizeof (int));
    zmq_close (pub);
    zmq_close (sub);
    zmq_term (ctx);
    return 0;
}

