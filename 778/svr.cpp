#include <zmq.hpp>
#include "zhelpers.hpp"

int main () {
    zmq::context_t context(1);


	std::string lInterAddr = "tcp://127.0.0.1:6666";


    zmq::socket_t lHarbor(context, ZMQ_ROUTER);
	lHarbor.setsockopt( ZMQ_IDENTITY, "POINT1", 6);

	int linger = 0;
	lHarbor.setsockopt (ZMQ_LINGER, &linger, sizeof (linger));

	int lvalue = 1;
	lHarbor.setsockopt( ZMQ_ROUTER_MANDATORY, &lvalue, sizeof(lvalue));
    lHarbor.bind( lInterAddr.c_str());
	//std::string lExternAddr  = "tcp://127.0.0.1:7777";
	// lHarbor.connect( lEcternAddr.c_str());

	bool lrs=false;

	while(1)
	{
		printf("Put any key to send another msg\n");
		getchar();
		lrs =  s_sendmore (lHarbor, "POINT2");
		lrs =  s_send (lHarbor, "Send From lPoint1");	
	}

	return 0;
}
