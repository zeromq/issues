//
// Disconnecting a socket from an unknown endpoint should
// return EINVAL. Instead, it returns ENOENT.
//
// gcc -lzmq issue.c
// chmod +x a.out
// ./a.out
//
#include <zmq.h>

#include <assert.h>
#include <stdio.h>



int main (void) 
{
      int rc;
      void *context = zmq_init (1);
      void *socket = zmq_socket (context, ZMQ_PUB);
      assert (-1 == zmq_disconnect (socket, "ipc:///tmp/some_ipc"));
      assert (ENOENT == zmq_errno () ); // succeeds but should fail
      assert (EINVAL == zmq_errno () ); // fails but should succeed
       
      zmq_close (socket);
      zmq_term (context);
      return (0);
}
