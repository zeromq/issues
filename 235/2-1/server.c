//
//  PUSH fails to load balance on windows
//  Binds PUSH socket to tcp://localhost:5557
//
// gcc -lzmq server.c -o server
// chmod +x server
// ./server
//
// on Windows, compile with:
// gcc -I<path to include files> -L<path to libzmq.dll> -lzmq server.c -o winserver
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

    //  Socket to send messages on
    void *sender = zmq_socket (context, ZMQ_PUSH);
    uint64_t hwm = 1;
    //zmq_setsockopt (sender, ZMQ_HWM, &hwm, sizeof (hwm));
    int linger = 0;
    zmq_setsockopt (sender, ZMQ_LINGER, &linger, sizeof (linger));
    zmq_bind (sender, "tcp://127.0.0.1:5557");

    //  Wait for threads to connect, since otherwise the messages
    //  we send will be dropped
    printf ("Press Enter when the workers are ready: ");
    getchar ();
    printf ("Sending tasks to workers...\n");
    

    int rc = 0;
    int i;
    int successful = 0;
    int eagain = 0;
    
    for(i = 0; i < 1000000; i++) {
      zmq_msg_t task;
      zmq_msg_init_data (&task, "Hello world", 11, NULL, NULL);
      rc = zmq_send (sender, &task, ZMQ_NOBLOCK);
      
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
