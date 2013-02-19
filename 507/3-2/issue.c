//
// A socket fails to unbind from a bound inproc endpoint.
// This works for other transport types.
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
      assert (0 == zmq_bind (socket, "inproc://some_addr"));
      assert (0 == zmq_unbind (socket, "inproc://some_addr"));
       
      zmq_close (socket);
      zmq_term (context);
      return (0);
}
