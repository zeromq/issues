#include <zmq.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

static void* client(void *context)
{
    int request_index = 0;
    while (1)
    {
        int rc;
        void *sender = zmq_socket (context, ZMQ_PAIR);
        rc = zmq_connect (sender, "inproc://identity");
        assert(rc != -1);
        char buffer [256];
        sprintf(buffer, "message [%d]", request_index);
        rc = zmq_send (sender, buffer, strlen(buffer) + 1, 0);
        assert(rc != -1);
        request_index++;
        int linger = 0;
        zmq_setsockopt(sender, ZMQ_LINGER, &linger, sizeof(int));
        rc = zmq_close (sender);
        assert (rc != -1);
    }
}

static void* server(void *context)
{
    void *receiver = zmq_socket (context, ZMQ_PAIR);
    zmq_bind (receiver, "inproc://identity");
    while (1)
    {
        char buffer [256];
        int rc = zmq_recv (receiver, buffer, 256, 0);
        assert(rc != -1);
        printf("message received: %s\n", buffer);
    }
}


int main (void)
{
    void *context = zmq_ctx_new ();
    pthread_t thread1, thread2;
    pthread_create (&thread1, NULL, server, context);
    pthread_create (&thread2, NULL, client, context);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    zmq_ctx_destroy (context);
    return 0;
}
