#include "RyuguNet.h"
#include "Debug.h"
#include "EventLoopThread.h"
#include "Net.h"
#include<iostream>

using namespace ck;
// Timer test1
void test1();
//Timer test2
void test2();

// Thread test
void test3();

// echo
void test4();

int main()
{
	test4();
	return 0;
}
/*

// 定时器测试
void test1()
{
	EventLoop loop;
	TcpServerPtr server=TcpServer::startServer(&loop,"",8080);
	server->setConnCb([&loop](const TcpConnPtr &conn) {
		  loop.runEvery(3, [=]() {conn->send("123"); });
		LOG("%d",conn->channel->getFd());
	});
	server->setReadCb([](const TcpConnPtr &conn) {
		conn->send(conn->getInput());
	});

	loop.loop();
}
void test2()
{
	EventLoop loop;
	loop.runEvery(1,[]{std::cout<<"hello"<<std::endl;});
	loop.runEvery(1,[]{std::cout<<"world"<<std::endl;});
	loop.loop();
}
void test3()
{
	EventLoopThread thread("hehe");
	thread.start();
	while(1);
}
*/
// 
void test4()
{
	EventLoop loop;
	// ten threads..
    net::Ipv4Addr listen_addr("",8080);
    TcpServer server(&loop,listen_addr,false);
    server.start();
	server.setMessageCb([](const TcpConnPtr &conn) {
		conn->send(conn->getInput());
	});

	loop.loop();
}
