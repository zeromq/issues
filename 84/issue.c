#include <zmq.h>
#include <assert.h>
 
// Use the following command to check for sockets left in TIME_WAIT
// due to creation of emulated socketpair in Windows.
//      netstat -an | grep 5905
 
// With the patch, this test program should run to completion and yield
// no additional TIME_WAIT entries
 
// Without the patch, "Address already in use (signaler.cpp)" assertion
// will occur and netstat will be filled with TIME_WAIT entries.
// Windows XP is more susceptible to this exception as it has less
// ephemeral ports than newer versions of Windows.
 
// On Windows XP, the program below will assert before loop iteration 1000
 
int main (void)
{
    int cnt = 0;
    for (cnt=0; cnt < 10000; cnt++) {
        // 3 signaling mailboxes are created per context
        void *ctx = zmq_ctx_new();
        assert (ctx);
        // 1 signaling mailbox is created per zmq socket
        void *sock = zmq_socket (ctx, ZMQ_DEALER);
        assert (sock);
        int rc = zmq_close (sock);
        assert (rc == 0);        
        rc = zmq_ctx_destroy(ctx);
        assert (rc == 0);        
       
        if (cnt && cnt % 100==0)
            printf("%d\n", cnt);
    }
    return 0;
}
