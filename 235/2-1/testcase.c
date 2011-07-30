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
#include <pthread.h>
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

static void *
worker_task_a (void *args)
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
    return (NULL);
}


int main (void) 
{
    void *context = zmq_init (1);

    //  Socket to send messages on
    void *sender = zmq_socket (context, ZMQ_PUSH);
    uint64_t hwm = 0;
    zmq_setsockopt (sender, ZMQ_HWM, &hwm, sizeof (hwm));
    int linger = 0;
    zmq_setsockopt (sender, ZMQ_LINGER, &linger, sizeof (linger));
    zmq_bind (sender, "tcp://*:5557");

    pthread_t worker;
    pthread_create (&worker, NULL, worker_task_a, NULL);
    pthread_create (&worker, NULL, worker_task_a, NULL);

    //  Wait for threads to connect, since otherwise the messages
    //  we send will be dropped
    sleep (1);

    printf ("Sending tasks to workers...\n");

    int rc = 0;
    int i;
    int successful = 0;
    int eagain = 0;
    
    for(i = 0; i < 1000000; i++) {
      zmq_msg_t task;
      zmq_msg_init_data (&task, "Hello world", 11, NULL, NULL);
      rc = zmq_send (sender, &task, 0/*ZMQ_NOBLOCK */);
      
      if (rc == 0) {
        successful++;
      }
      else {
        eagain++;
      }
      
      zmq_msg_close (&task);
    }
    
    printf("Successfully sent [%d] messages\n", successful);
    printf("EAGAIN received for [%d] messages\n", eagain);
    zmq_close (sender);
    zmq_term (context);
    return 0;
}
