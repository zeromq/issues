//
//  PUSH fails to load balance on windows
//  Binds PUSH socket to tcp://localhost:5557
//
// gcc -lzmq testcase.c
// chmod +x a.out
// ./a.out
//
#include <zmq.h>

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
      void *context = zmq_init (1);
      void *worker = zmq_socket (context, ZMQ_PULL);
      int linger = 0;
      zmq_setsockopt (worker, ZMQ_LINGER, &linger, sizeof (linger));
      zmq_connect (worker, "tcp://127.0.0.1:5557");
      printf("worker has started\n");

      int total = 0;
      int rc = 0;
      while (1) {
          zmq_msg_t message;
          zmq_msg_init (&message);

          // wait for a message
          //if ((rc = zmq_recv (worker, &message, ZMQ_NOBLOCK)) == 0) {
          if ((rc = zmq_recv (worker, &message, 0)) == 0) {
            // print message
            //printf("worker got message\n");
          }
  //        else {
  //          printf("worker, rc != 0, [%d]\n", rc);
  //        }
          zmq_msg_close (&message);
      }
      zmq_close (worker);
      zmq_term (context);
      return (0);
}
