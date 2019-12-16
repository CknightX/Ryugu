#include "cknet.h"
#include "debug.h"
#include<iostream>

using namespace ck;

void test1();
void test2();

int main()
{
	test2();
	return 0;
}

void test1()
{
	EventHandler handler;
	TcpServerPtr server=TcpServer::startServer(&handler,"",8080);
	server->setConnRead([](const TcpConnPtr &conn) {
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