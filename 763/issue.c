//
//  Raw test case for issue #763
//  When run, shows the received frames

#include <czmq.h>

//  This is a ZMTP v1 message
typedef struct {
    byte size;              //  Size, 1 to 255 bytes
    byte flags;             //  Must be zero
    byte data [255];        //  Message data
} zmtp_msg_t;

static void
derp (char *s)
{
    perror (s);
    exit (1);
}

static void
tcp_send (int handle, void *buffer, size_t len)
{
    if (send (handle, buffer, len, 0) == -1)
        derp ("send");
}

static void
tcp_recv (int handle, void *buffer, size_t len)
{
    size_t len_recd = 0;
    while (len_recd < len) {
        ssize_t bytes = recv (handle, buffer + len_recd, len - len_recd, 0);
        if (bytes == -1)
            derp ("recv");
        len_recd += bytes;
    }
    printf ("%zd: ", len_recd);
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < len_recd; byte_nbr++)
        printf ("%02X ", ((byte *) buffer) [byte_nbr]);
    printf ("\n");
}

static void
zmtp_recv (int handle, zmtp_msg_t *msg)
{
    puts ("-------------------");
    tcp_recv (handle, msg, 1);
    if (msg->size == 255) {
        byte long_len [8];
        tcp_recv (handle, long_len, 8);
        msg->size = long_len [7];
    }
    msg->size--;        //  ZMTP size includes flags
    tcp_recv (handle, &msg->flags, 1);
    if (msg->size)
        tcp_recv (handle, msg->data, msg->size);
}

static void
zmtp_send (int handle, zmtp_msg_t *msg)
{
    msg->size++;        //  ZMTP size includes flags
    tcp_send (handle, msg, msg->size);
}

void *
dealer_task (void *args)
{
    zctx_t *ctx = zctx_new ();
    void *dealer = zsocket_new (ctx, ZMQ_DEALER);
    zmq_connect (dealer, "tcp://127.0.0.1:5555");
    zstr_send (dealer, "12345");
    sleep (1);
    zctx_destroy (&ctx);
    return NULL;
}


int main (void)
{
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf ("Current 0MQ version is %d.%d.%d\n", major, minor, patch);

    //  Start background task to act as client
    zctx_t *ctx = zctx_new ();
    zthread_new (dealer_task, NULL);

    //  Create TCP socket
    int listener;
    if ((listener = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        derp ("socket");

    //  Wait for incoming connections on port 5555
    struct sockaddr_in si_this = { 0 };
    si_this.sin_family = AF_INET;
    si_this.sin_port = htons (5555);
    si_this.sin_addr.s_addr = htonl (INADDR_ANY);
    int reuse = 1;
    setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (int));
    if (bind (listener, &si_this, sizeof (si_this)) == -1)
        derp ("bind");

    if (listen (listener, 1) == -1)
        derp ("listen");

    //  Wait for one connection and handle it
    int peer;
    if ((peer = accept (listener, NULL, NULL)) == -1)
        derp ("accept");

    //  Send anonymous ZMTP V1 handshake to peer
    zmtp_msg_t msg = { 1, 0 };
    zmtp_send (peer, &msg);

    while (!zctx_interrupted)
        zmtp_recv (peer, &msg);

    close (peer);
    zctx_destroy (&ctx);
    return 0;
}
