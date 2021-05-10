#include "RyuguNet.h"
#include "Debug.h"
#include "EventLoopThread.h"
#include "Net.h"
#include "InetAddr.h"
#include "TcpConn.h"
#include<iostream>
#include <string>

using namespace ryugu;
using namespace ryugu::net;
using std::string;
// echo
void test1();
// 主动关闭
void test2();

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
	server.setMessageCb([&loop](const TcpConnPtr &conn) {
		string mes = conn->getInput();
		std::cout << mes.size();
		if (mes == "quit\r\n")
		{
			loop.stop();
		}
		conn->send(mes);
	});
	server.setThreadNum(5);
    server.start();
	loop.loop();
}
void test2()
{
	EventLoop loop;
    net::InetAddr listen_addr("",8081);
    TcpServer server(&loop,listen_addr,false);
	server.setMessageCb([](const TcpConnPtr &conn) {
        conn->forceClose();
	});
	server.setThreadNum(5);
    server.start();

	loop.loop();
}
