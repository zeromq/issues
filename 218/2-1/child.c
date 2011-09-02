//  Child test, run from within parent

#include "zmq.h"
#include "assert.h"
#include "stdint.h"
#include "stdio.h"
#include "unistd.h"

int main ()
{
    void *ctx = zmq_init (1);
    assert (ctx);

    void *dealer = zmq_socket (ctx, ZMQ_DEALER);
    assert (dealer);
    int rc = zmq_connect (dealer, "tcp://localhost:2211");
    assert (rc == 0);

    while (1) {
        //  Send signal to parent
        zmq_msg_t msg;
        zmq_msg_init (&msg);
        zmq_send (dealer, &msg, 0);
        zmq_msg_close (&msg);

        //  Do some work
        sleep (1);
    }
    return 0;
}

