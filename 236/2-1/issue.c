#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (void)
{
    int rc;
    char data[5] = "hello";
    zmq_msg_t msg;
    void *context = zmq_init (1);

    zmq_msg_init_data (&msg, &data, 5, NULL, NULL);
    void *rep = zmq_socket (context, ZMQ_REP);
    void *req = zmq_socket (context, ZMQ_REQ);
    zmq_bind (rep, "tcp://127.0.0.1:5555");
    zmq_connect (req, "tcp://127.0.0.1:5555");

    // proper error
    printf ("rep.send 1\n");
    rc = zmq_send (rep, &msg, 0);
    if (rc)
        printf ("error1 %d: %s\n", rc, zmq_strerror (zmq_errno ()));
        
    // this will block:
    printf ("rep.send 2\n");
    rc = zmq_send (rep, &msg, 0);
    if (rc)
        printf ("error2 %d: %s\n", rc, zmq_strerror (zmq_errno ()));

    // the same is true for REQ.recv
    printf ("req.recv 1\n");
    rc = zmq_recv (req, &msg, 0);
    if (rc)
        printf ("%d: %s\n", rc, zmq_strerror (zmq_errno ()));
    
    printf ("req.recv 2\n");
    rc = zmq_recv (req, &msg, 0);
    if (rc)
        printf ("%d: %s\n", rc, zmq_strerror (zmq_errno ()));

    zmq_close (req);
    zmq_close (rep);
    zmq_term (context);
    return 0;
}