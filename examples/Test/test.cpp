#include <iostream>
#include <string>
#include "Ryugu/base/Debug.h"
#include "Ryugu/net/EventLoopThread.h"
#include "Ryugu/net/EventLoop.h"
#include "Ryugu/net/InetAddr.h"
#include "Ryugu/net/TcpConn.h"
#include "Ryugu/net/TcpServer.h"

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
    net::InetAddr listen_addr(8001);
    TcpServer server(&loop,listen_addr,true);
	server.setMessageCb([&loop](const TcpConnPtr &conn) {
		string mes = conn->getInput();
		if (mes == "quit\r\n")
		{
			loop.stop();
		}
		conn->send(mes);
	});
	server.setThreadNum(1);
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
