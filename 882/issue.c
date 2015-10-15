
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zmq.h>
#include <zmq_utils.h>


/*
 *  Receive a string from socket and return a copy (must be freed by caller)
 *  or NULL, if an error occurred.
 */
static char *s_tryrecv(void *socket)
{
    char buffer[256];
    int size;

    if ((size = zmq_recv(socket, buffer, sizeof(buffer), 0)) == -1)
        return NULL;
    if (size >= sizeof(buffer))
        size = sizeof(buffer) - 1;
    buffer[size] = '\0';
    return strdup(buffer);
}


/*
 *  Receive a string from socket and return a copy (must be freed by caller).
 */
static char *s_recv(void *socket)
{
    char *str;

    assert((str = s_tryrecv(socket)));
    return str;
}


/*
 *  Return non-zero if socket has more data, zero otherwise.
 */
static int has_more(void *socket)
{
    int more;
    size_t size;

    size = sizeof(more);
    assert(!zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &size));
    return more;
}


/*
 *  Assure that socket has more data before receiving and returning a string.
 */
static char *s_recvmore(void *socket)
{
    assert(has_more(socket));
    return s_recv(socket);
}


/*
 *  Send a string (possibly the last in a multipart message).
 */
static void s_send(void *socket, const char *str)
{
    assert(zmq_send(socket, str, strlen(str), 0) != -1);
}


/*
 *  Send a string in a frame of a multipart message.
 */
static void s_sendmore(void *socket, const char *str)
{
    assert(zmq_send(socket, str, strlen(str), ZMQ_SNDMORE) != -1);
}


/*
 *  ZAP request handling loop.
 */
static void zap_handler(void *handler)
{
    for (;;) {
        char *version, *sequence, *domain, *address, *identity, *mechanism;

        /* Receive request. */
        if(!(version = s_tryrecv(handler)))
            break;
        sequence = s_recvmore(handler);
        domain = s_recvmore(handler);
        address = s_recvmore(handler);
        identity = s_recvmore(handler);
        mechanism = s_recvmore(handler);
        assert(!has_more(handler));

        assert(!strcmp(version, "1.0"));
        assert(!strcmp(mechanism, "NULL"));

        printf("ZAP REQUEST %s %s %s %s %s %s\n", version, sequence, domain,
                address, identity, mechanism);
        
        /* Send reply. */
        s_sendmore(handler, version);
        s_sendmore(handler, sequence);
        if(!strcmp(domain, "test")) {
            s_sendmore(handler, "200");
            s_sendmore(handler, "OK");
            s_sendmore(handler, "anonymous");
            printf("ZAP REPLY 1.0 %s 200 OK anonymous\n", sequence);
        } else {
            s_sendmore(handler, "500");
            s_sendmore(handler, "Denied");
            s_sendmore(handler, "");
            printf("ZAP REPLY 1.0 %s 200 Denied\n", sequence);
        }
        s_send(handler, "");

        /* Cleanup. */
        free(version);
        free(sequence);
        free(domain);
        free(address);
        free(identity);
        free(mechanism);
    }
    zmq_close(handler);
}



int main(int argc, char *argv[])
{
    void *ctx, *handler, *thread, *server, *client;
    const char *domain, *connect_addr;
    int optval;

    domain = argc > 1 ? argv[1] : "test";
    connect_addr = strcmp(domain, "fail") ? "tcp://127.0.0.1:9000" : "tcp://127.0.0.1:9001";

    assert((ctx = zmq_ctx_new()));

    /* Start ZAP handler thread. */
    assert((handler = zmq_socket(ctx, ZMQ_REP)));
    assert(!(zmq_bind(handler, "inproc://zeromq.zap.01")));
    assert((thread = zmq_threadstart(zap_handler, handler)));

    /* Bind server. */
    assert((server = zmq_socket(ctx, ZMQ_DEALER)));
    assert(!(zmq_setsockopt(server, ZMQ_ZAP_DOMAIN, domain, strlen(domain))));
    assert(!zmq_bind(server, "tcp://127.0.0.1:9000"));

    /* Connect client. */
    assert((client = zmq_socket(ctx, ZMQ_DEALER)));
    optval = 200;
    assert(!(zmq_setsockopt(client, ZMQ_RECONNECT_IVL, &optval, sizeof(optval))));
    optval = 5000;
    assert(!(zmq_setsockopt(client, ZMQ_RECONNECT_IVL_MAX, &optval, sizeof(optval))));
    optval = 30000;
    assert(!(zmq_setsockopt(client, ZMQ_SNDTIMEO, &optval, sizeof(optval))));
    optval = 1;
    assert(!(zmq_setsockopt(client, ZMQ_IMMEDIATE, &optval, sizeof(optval))));
    assert(!zmq_connect(client, connect_addr));

    /* Bounce test. */
    s_send(client, "Hello, Server!");
    assert(!strcmp(s_recv(server), "Hello, Server!"));
    s_send(server, "Hello, Client!");
    assert(!strcmp(s_recv(client), "Hello, Client!"));

    /* Cleanup. */
    assert(!zmq_disconnect(client, connect_addr));
    assert(!zmq_close(client));

    assert(!zmq_unbind(server, "tcp://127.0.0.1:9000"));
    assert(!zmq_close(server));

    assert(!zmq_term(ctx));
    zmq_threadclose(thread);

    return 0;
}

