/*
    Copyright (c) 2010-2011 250bpm s.r.o.
    Copyright (c) 2011 iMatix Corporation
    Copyright (c) 2010-2011 Other contributors as noted in the AUTHORS file

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

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <assert.h>
#include <cstring>
int main (void)
{
    fprintf (stderr, "test_router_mandatory2 running...\n");

    void *ctx = zmq_init (1);
    assert (ctx);

    // Creating the first socket.
    void *sa = zmq_socket (ctx, ZMQ_ROUTER);
    assert (sa);
    
    int rc = zmq_bind (sa, "tcp://127.0.0.1:15560");
    assert (rc == 0);

    // Sending a message to an unknown peer with the default setting
    rc = zmq_send (sa, "SOCKET", 6, ZMQ_SNDMORE);
    assert (rc == 6);
    rc = zmq_send (sa, "DATA", 4, 0);
    assert (rc == 4);

    int mandatory = 1;
    // Set mandatory routing on socket
    rc = zmq_setsockopt (sa, ZMQ_ROUTER_MANDATORY, &mandatory, sizeof (mandatory));
    assert (rc == 0);

    // Send a message and check that it fails
    rc = zmq_send (sa, "SOCKET", 6, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    assert (rc == -1 && errno == EHOSTUNREACH);

		//create a receiver socket
		void *receiver = zmq_socket (ctx, ZMQ_ROUTER);
    assert (receiver);
		//give it a name
		rc = zmq_setsockopt (receiver, ZMQ_IDENTITY, "SOCKET", 6);
  	assert (rc == 0);
		//connect to sender
		rc = zmq_connect (receiver, "tcp://127.0.0.1:15560");
  	assert (rc == 0);
		//sleep to connect
		zmq_sleep(1);
	
		 // Send a message and check that it NOT fails
    rc = zmq_send (sa, "SOCKET", 6, ZMQ_SNDMORE);
    assert (rc == 6);
		rc = zmq_send (sa, "DATA", 4, 0);
    assert (rc == 4);

//sleep to receive
		zmq_sleep(1);
		//receive it on socket
		//TODO:
		char buffer[16];
		memset (&buffer, 0, sizeof(buffer));
		rc =  zmq_recv (receiver, &buffer, sizeof(buffer), ZMQ_DONTWAIT);
		assert(rc > 0);
		rc =  zmq_recv (receiver, &buffer, sizeof(buffer), ZMQ_DONTWAIT);
		printf ("rc is %d, in buffer: %s \n",rc, buffer);
		assert(rc == 4);

		//close receiver socket
		rc = zmq_close(receiver);
		assert (rc == 0);

		//sleep to disconnect
		zmq_sleep(5);
		//send again to check if fails
  	// Send a message and check that it fails
    rc = zmq_send (sa, "SOCKET", 6, ZMQ_SNDMORE | ZMQ_DONTWAIT);
		printf ("rc is %d\n", rc);
    assert (rc == -1 && errno == EHOSTUNREACH);



		//closing stuff
    rc = zmq_close (sa);
    assert (rc == 0);

	
    rc = zmq_term (ctx);
    assert (rc == 0);

    return 0 ;
}
