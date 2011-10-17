//
// A SUB socket should always have ZMQ_POLLIN set
// when it has a message in queue and ready to read.
// FAILS when the SUB socket binds the socket.
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
      void *sender = zmq_socket (context, ZMQ_PUB);
      void *receiver = zmq_socket (context, ZMQ_SUB);
      assert (0 == zmq_setsockopt (receiver, ZMQ_SUBSCRIBE, "", 0));
      assert (0 == zmq_bind (receiver, "tcp://127.0.0.1:7557"));
      assert (0 == zmq_connect (sender, "tcp://127.0.0.1:7557"));
      sleep (1);

      zmq_msg_t m1, m2;
      assert (0 == zmq_msg_init_data (&m1, "Hello world", 11, NULL, NULL));
      assert (11 == zmq_sendmsg(sender, &m1, 0));
      sleep (1);

      unsigned int events;
      size_t size = sizeof (events);
      assert (0 == zmq_getsockopt(receiver, ZMQ_EVENTS, &events, &size));

      // the test!
      fprintf (stderr, "events [%d], pollin [%d]\n", events, ZMQ_POLLIN);
      assert (events == ZMQ_POLLIN);
       
      zmq_close (sender);
      zmq_close (receiver);
      zmq_term (context);
      return (0);
}
