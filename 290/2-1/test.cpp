#include <cstdio>
#include <zmq.hpp>

using namespace std;

int main()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_DEALER);
    socket.bind("inproc://abc123");

    zmq::pollitem_t items[1];
    items[0].socket = socket;
    items[0].events = ZMQ_POLLIN;

    while (true)
    {
        int rc = zmq::poll(items, 1, 1000 * 1000);
        if (rc == 0)
            printf("Poll timed out...\n");
        else
            break;
    }
}
