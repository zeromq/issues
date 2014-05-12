#include <cassert>
#include <iostream>
#include <string>
#include <zmq.h>

using std::cout;
using std::endl;
using std::string;

int main(int argc, char**)
{
    char const* addr;
    if((argc-1) % 2) {
        addr = "inproc://address";
        cout << "Test uses inproc:// transport" << endl;
    } else {
        addr = "tcp://127.0.0.1:1234";
        cout << "Test uses tcp:// transport" << endl;
    }

    cout << "Setting up sender" << endl;
    void* ctx = zmq_ctx_new();
    assert(ctx != NULL);
    void* sndr = zmq_socket(ctx, ZMQ_DEALER);
    assert(sndr != NULL);
    assert(0 == zmq_connect(sndr, addr));
    cout << "Sender set up, ";

    void* rcvr;
    if((argc-1) % 4 < 2) {
        cout << "setting up receiver" << endl;
        rcvr = zmq_socket(ctx, ZMQ_ROUTER);
        assert(rcvr != NULL);
        assert(0 == zmq_bind(rcvr, addr));
        cout << "Receiver set up, ";
    }

    if((argc-1) % 8 >= 4) {
        cout << "sending messages w/ non-empty envelope" << endl;
        assert(9 == zmq_send(sndr, "socket-ID", 9, ZMQ_SNDMORE));
    } else {
        cout << "sending messages w/ empty envelope" << endl;
    }
    assert(0 == zmq_send(sndr, "", 0, ZMQ_SNDMORE));
    assert(12 == zmq_send(sndr, "Message data", 12, 0));
    if((argc-1) % 8 >= 4)
        assert(9 == zmq_send(sndr, "socket-ID", 9, ZMQ_SNDMORE));
    assert(0 == zmq_send(sndr, "", 0, ZMQ_SNDMORE));
    assert(12 == zmq_send(sndr, "Message data", 12, 0));
    cout << "Messages sent, ";

    if((argc-1) % 4 >= 2) {
        cout << "setting up receiver" << endl;
        rcvr = zmq_socket(ctx, ZMQ_ROUTER);
        assert(rcvr != NULL);
        assert(0 == zmq_bind(rcvr, addr));
        cout << "Receiver set up, ";
    }

    cout << "receiving messages" << endl;
    for(int i=0; i<2; ++i) {
        cout << "Receiving message " << i+1 << ":" << endl;
        int more;
        do {
            char buffer[100];
            int n = zmq_recv(rcvr, buffer, sizeof(buffer), 0);
            assert(n <= sizeof(buffer));
            cout << "Got: \"" << string(buffer, n) << "\"" << endl;
            size_t size = sizeof(more);
            assert(0 == zmq_getsockopt(rcvr, ZMQ_RCVMORE, &more, &size));
        } while(more);
    }
    cout << "Done with sockets, ";

    cout << "cleaning up" << endl;
    assert(0 == zmq_close(sndr));
    assert(0 == zmq_close(rcvr));
    assert(0 == zmq_ctx_term(ctx));
    cout << "Finished test" << endl;

    return 0;
}
