#include <zmq.hpp>
#include "zhelpers.hpp"

int main () {
    zmq::context_t context(1);	
	
	std::string lExternAddr = "tcp://127.0.0.1:6666";//"inproc://example"
 
    zmq::socket_t lHarbor(context, ZMQ_ROUTER);


	{
		lHarbor.setsockopt( ZMQ_IDENTITY, "POINT2", 6);
		int lvalue = 1;
		lHarbor.setsockopt( ZMQ_ROUTER_MANDATORY, &lvalue, sizeof(lvalue));

		int linger = 0;
		lHarbor.setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
	}

	//std::string lInterAddr = "tcp://127.0.0.1:7777";//"inproc://example"
	//lPoint2.bind(lInterAddr.c_str());
    lHarbor.connect( lExternAddr.c_str());


	while(1)
	{
		printf("wait for another msg......");
		s_dump (lHarbor);
    }

    return 0;
}
