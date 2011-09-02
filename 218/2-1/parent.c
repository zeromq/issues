//  Parent test, run twice in a row to provoke error

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
    int rc = zmq_bind (dealer, "tcp://*:2211");
    assert (rc == 0);

    //  Start child process
    pid_t pid = fork();
    if (pid == 0)
        execl ("child", "child",NULL);

    zmq_msg_t msg;
    zmq_msg_init (&msg);
    zmq_recv (dealer, &msg, 0);
    zmq_msg_close (&msg);
    
    int linger = 0;
    zmq_close (dealer);
    zmq_term (ctx);
    return 0;
}

