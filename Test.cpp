#include "RyuguNet.h"
#include "Debug.h"
#include "EventLoopThread.h"
#include "Net.h"
#include "InetAddr.h"
#include "TcpConn.h"
#include<iostream>

using namespace ryugu;
using namespace ryugu::net;
// echo
void test1();

int main()
{
	test1();
	return 0;
}


void test1()
{
	EventLoop loop;
	// ten threads..
    net::InetAddr listen_addr("",8081);
    TcpServer server(&loop,listen_addr,false);
	server.setMessageCb([](const TcpConnPtr &conn) {
		conn->send(conn->getInput());
	});
	server.setThreadNum(20);
    server.start();

	loop.loop();
}
