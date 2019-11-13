#include "cknet.h"
#include "debug.h"
#include<iostream>

using namespace ck;

int main()
{
	EventHandler handler;
	TcpServerPtr server=TcpServer::startServer(&handler,"",8080);
	server->setConnRead([](const TcpConnPtr& conn){
		 conn->send(conn->getInput());
		LOG("msg:%s",conn->getInput().c_str());
	});

	handler.loop();
	
}