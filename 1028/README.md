Issue 1028 Description
======================

The `issue1028.cxx` test code sends two multi-part messages from a
`ZMQ_DEALER` to a `ZMQ_ROUTER` socket and outputs the message parts as they
are received. The `ZMQ_ROUTER` is bound, the `ZMQ_DEALER` connects.

Several properties of the test can be selected by the number of command
line arguments *N*: the argument values are inconsequential, just the
number of arguments is used. There are eight different configurations for
the test, using eight (or more) arguments just wraps back to zero.

* *N* = 0, 2, 4, 6: Use `tcp://` transport
* *N* = 1, 3, 5, 7: Use `inproc://` transport
* *N* = 0, 1, 4, 5: Create and bind `ZMQ_ROUTER` before sending messages
* *N* = 2, 3, 6, 7: Send messages before creating and binding `ZMQ_ROUTER`
* *N* = 0, 1, 2, 3: Send empty envelopes (first message part is zero)
* *N* = 4, 5, 6, 7: Send one-hop envelopes (first message part is non-zero)

The issue becomes visible for 3 or 7 arguments: The first message part of
the first message is missing from the dump (the first "Got:"-lines are
the socket IDs assigned by the `ZMQ_ROUTER`, so those might be different
from what is shown here):

    Test uses inproc:// transport
    Setting up sender
    Sender set up, sending messages w/ empty envelope
    Messages sent, setting up receiver
    Receiver set up, receiving messages
    Receiving message 1:
    Got: " Ç  )"
                         <== Where is the envelope separator?
    Got: "Message data"
    Receiving message 2:
    Got: " Ç  )"
    Got: ""
    Got: "Message data"
    Done with sockets, cleaning up
    Finished test

For 7 arguments, it becomes obvious that the missing message part replaces
the socket identification:

    Test uses inproc:// transport
    Setting up sender
    Sender set up, sending messages w/ non-empty envelope
    Messages sent, setting up receiver
    Receiver set up, receiving messages
    Receiving message 1:
    Got: "socket-ID"     <== This was the first message part
                         <== Again, the first message part is missing
    Got: ""
    Got: "Message data"
    Receiving message 2:
    Got: "socket-ID"     <== The weird socket ID is used from now on
    Got: "socket-ID"
    Got: ""
    Got: "Message data"
    Done with sockets, cleaning up
    Finished test
