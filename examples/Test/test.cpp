#include "Ryugu/RyuguNet.h"
#include "Ryugu/Debug.h"
#include "Ryugu/EventLoopThread.h"
#include "Ryugu/Net.h"
#include "Ryugu/InetAddr.h"
#include "Ryugu/TcpConn.h"
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
		string header = "";
		header += "HTTP/1.1 200 OK\r\n";
		header += "Content-type: text/html\r\n";
		header += "Content-length: 1\r\n";
		header += "Connection: keep-alive\r\n";
		header += "\r\n";
		header += "c";
		conn->send(header);
		//conn->send(mes);
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
