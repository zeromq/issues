#include "testutil.hpp"

static void pusher (void *ctx)
{
    // Connect first
    void *connectSocket = zmq_socket (ctx, ZMQ_PAIR);
    assert (connectSocket);
    int rc = zmq_connect (connectSocket, "inproc://a");
    assert (rc == 0);

    // Queue up some data
    rc = zmq_send_const (connectSocket, "foobar", 6, 0);
    assert (rc == 6);

    // Cleanup
    rc = zmq_close (connectSocket);
    assert (rc == 0);
}

int main (void)
{
    setup_test_environment ();

    const unsigned int no_of_threads = 30;
    while (true)
    {
        void *ctx = zmq_ctx_new ();
        int rc;
        void *threads [no_of_threads];

        // Connect first
        for (unsigned int i = 0; i < no_of_threads; ++i)
        {
            threads [i] = zmq_threadstart (&pusher, ctx);
        }

        // Now bind
        void *bindSocket = zmq_socket (ctx, ZMQ_PULL);
        assert (bindSocket);
        rc = zmq_bind (bindSocket, "inproc://a");
        assert (rc == 0);

        for (unsigned int i = 0; i < no_of_threads; ++i)
        {
            // Read pending message
            zmq_msg_t msg;
            zmq_msg_init (&msg);
            zmq_msg_recv (&msg, bindSocket, 0);
            void *data = zmq_msg_data (&msg);
            assert (memcmp ("foobar", data, 6) == 0);
        }

        // Cleanup
        for (unsigned int i = 0; i < no_of_threads; ++i)
        {
            zmq_threadclose (threads [i]);
        }

        rc = zmq_close (bindSocket);
        assert (rc == 0);

        rc = zmq_ctx_term (ctx);
        assert (rc == 0);
    }

    return 0;
}