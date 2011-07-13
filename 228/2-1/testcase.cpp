#include <stdint.h>
#include <iostream>

#include <zmq.hpp>

int
main()
{
  char endpoint[] = "inproc://link";
  zmq::context_t ctx(1);
  uint64_t one = 1;
  uint64_t many = 100;

  zmq::socket_t s(ctx, ZMQ_XREQ);
  s.setsockopt(ZMQ_HWM, &one, sizeof(uint64_t));
  s.bind(endpoint);

  zmq::socket_t r(ctx, ZMQ_REP);
  r.setsockopt(ZMQ_HWM, &one, sizeof(uint64_t));
  r.connect(endpoint);

  zmq::message_t empty;
  s.send(empty, ZMQ_SNDMORE);
  zmq::message_t something1(10);
  s.send(something1);
  std::cout << "X sent first" << std::endl;

  s.send(empty, ZMQ_SNDMORE);
  zmq::message_t something2(10);
  s.send(something2);
  std::cout << "X sent second" << std::endl;

  zmq::message_t area;
  r.recv(&area);
  std::cout << "received first" << std::endl;

  s.send(empty, ZMQ_SNDMORE);
  zmq::message_t something3(10);
  s.send(something3);
  std::cout << "X sent third" << std::endl;

  r.send(area);
  std::cout << "sent first" << std::endl;


  r.recv(&area);
  std::cout << "received second" << std::endl;

  r.send(area);
  std::cout << "sent second" << std::endl;

  r.recv(&area);
  std::cout << "received third" << std::endl;

  r.send(area);
  std::cout << "sent third" << std::endl;
}
