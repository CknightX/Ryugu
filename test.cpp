#include "Ryugu.h"
#include "debug.h"
#include "EventHandlerThread.h"
#include<iostream>

using namespace ck;
// Timer test1
void test1();
//Timer test2
void test2();

// Thread test
void test3();

int main()
{
	test3();
	return 0;
}

void test1()
{
	EventHandler handler;
	TcpServerPtr server=TcpServer::startServer(&handler,"",8080);
	server->setConnCb([&handler](const TcpConnPtr &conn) {
		  handler.runEvery(3, [=]() {conn->send("123"); });
		LOG("%d",conn->channel->getFd());
	});
	server->setReadCb([](const TcpConnPtr &conn) {
		conn->send(conn->getInput());
	});

	handler.loop();
}
void test2()
{
	EventHandler handler;
	handler.runEvery(1,[]{std::cout<<"hello"<<std::endl;});
	handler.runEvery(1,[]{std::cout<<"world"<<std::endl;});
	handler.loop();
}
void test3()
{
	EventHandlerThread thread("hehe");
	thread.start();
	while(1);
}