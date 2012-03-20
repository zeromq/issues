/*
    Copyright (c) 2007-2011 iMatix Corporation
    Copyright (c) 2007-2011 Other contributors as noted in the AUTHORS file

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "czmq.h"

int main (int argc, char *argv [])
{
    void *context = zmq_ctx_new ();
    void *pusher = zmq_socket (context, ZMQ_PUSH);

    zmq_bind (pusher, "tcp://127.0.0.1:5858");
    
    //  Connect normal socket to 0MQ PUSH socket
    int puller = socket (AF_INET, SOCK_STREAM, 0);
    assert (puller >= 0);
    
    struct sockaddr_in addr;
    memset (&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
    addr.sin_port = htons (5858);
    int rc = connect (puller, (struct sockaddr *) &addr, sizeof (addr));
    assert (rc == 0);
    
    //  Send ZMTP greeting message
    unsigned char buf [512];
    buf [0] = 0x01;
    buf [1] = 0x00;
    rc = send (puller, buf, 2, 0);
    assert (rc == 2);

    //  Send a message just to ensure there's something to read
    zmq_msg_t msg;
    zmq_msg_init  (&msg);
    zmq_msg_send  (&msg, pusher, 0);
    zmq_msg_close (&msg);

    //  Read raw ZMTP frame from normal socket
    rc = recv (puller, buf, 512, 0);
    assert (rc >= 2);

    //  Small or large frame?
    int flags_off = 1;
    if (buf [0] == 255) {
        assert (rc >= 10);
        flags_off = 9;
    }
    //  Check reserved flags are set to zero
    assert (buf [flags_off] == 0);

    zmq_close (pusher);
    zmq_ctx_destroy (context);

    return 0;
}
