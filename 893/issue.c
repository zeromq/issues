#include <assert.h>
#include <zmq.h>

#define n 28  /* n = 27 works */
#define m n

int main(void)
{
    void* ctx;
    void* sckts[n];
    int rc;
    int i;

    assert(m <= n);

    printf("#       : create context\n");
    ctx = zmq_ctx_new();
    assert(ctx);

    rc = zmq_ctx_get(ctx, ZMQ_MAX_SOCKETS);
    assert(rc >= n);

    printf("##      : starting to create sockets\n");
    for(i=0; i<n; ++i) {
        if(i == 0)
            sckts[i] = zmq_socket(ctx, ZMQ_REP);
        else
            sckts[i] = zmq_socket(ctx, ZMQ_REQ);
        assert(sckts[i]);
    }

    printf("####    : starting to connect sockets\n");
    for(i=0; i<m; ++i) {
        if(i == 0)
            rc = zmq_bind(sckts[i], "inproc://test");
        else
            rc = zmq_connect(sckts[i], "inproc://test");
        assert(rc == 0);
    }

    printf("######  : starting to close sockets\n");
    for(i=0; i<n; ++i) {
        rc = zmq_close(sckts[i]);
        assert(rc == 0);
    }

    printf("####### : destroy context\n");
    rc = zmq_ctx_destroy(ctx);
    assert(rc == 0);

    printf("Success : all resources released\n");
    return 0;
}
