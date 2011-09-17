//
//  XREP sockets will receive multiple message
//  parts. The leading parts are labels (routing
//  info) while non-label parts are user content.
//  Label parts should return TRUE for ZMQ_RCVMORE.
//
// gcc -lzmq issue.c
// chmod +x a.out
// ./a.out
//
//#include <zmq.h>
#include "include/zmq.h"

// several of these are probably unnecessary...
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>



int main (void) 
{
      int rc;
      void *context = zmq_init (1);
      void *sender = zmq_socket (context, ZMQ_REQ);
      void *receiver = zmq_socket (context, ZMQ_XREP);
      assert (0 == zmq_bind (receiver, "tcp://127.0.0.1:7557"));
      assert (0 == zmq_connect (sender, "tcp://127.0.0.1:7557"));

      zmq_msg_t m1, m2;
      assert (0 == zmq_msg_init_data (&m1, "Hello world", 11, NULL, NULL));
      assert (11 == zmq_sendmsg(sender, &m1, 0));

      assert (0 == zmq_msg_init (&m2) );
      assert (-1 != zmq_recvmsg (receiver, &m2, 0));

      int label, more;
      size_t size = sizeof (label);
      assert (0 == zmq_getsockopt (receiver, ZMQ_RCVLABEL, &label, &size));
      assert (0 == zmq_getsockopt (receiver, ZMQ_RCVMORE, &more, &size));
       
      printf ("RCVLABEL %d, RCVMORE %d (should be 1)\n", label, more);
       
      zmq_close (sender);
      zmq_close (receiver);
      zmq_term (context);
      return (0);
}
