#include <stdint.h>
#include <iostream>

#include "zmq.h"
#include "zmq.hpp"

int
main()
{
  char endpoint[] = "inproc://link";
  zmq::context_t ctx(1);
  int one = 1;
  int many = 100;

  zmq::socket_t s(ctx, ZMQ_XREQ);
  s.setsockopt(ZMQ_RCVHWM, &one, sizeof(one));
  s.setsockopt(ZMQ_SNDHWM, &one, sizeof(one));
  s.bind(endpoint);

  zmq::socket_t r(ctx, ZMQ_REP);
  r.setsockopt(ZMQ_RCVHWM, &one, sizeof(one));
  r.setsockopt(ZMQ_SNDHWM, &one, sizeof(one));
  r.connect(endpoint);

  //  Send Req 1
  zmq::message_t something1(10);
  s.send(something1);

  //  Send Req 2
  zmq::message_t something2(10);
  s.send(something2);

  //  Recv Req 1
  zmq::message_t area;
  r.recv(&area);

  //  Send Req 3
  zmq::message_t something3(10);
  s.send(something3);

  //  Send Rep 1
  r.send(area);

  //  Recv Req 2
  r.recv(&area);

  //  Send Rep 2
  r.send(area);

  //  Recv Req 3
  r.recv(&area);

  //  Send Rep 3
  r.send(area);
}
