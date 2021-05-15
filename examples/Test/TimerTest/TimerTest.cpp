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
int main()
{
	EventLoop loop;
    net::InetAddr listen_addr(8000);
    TcpServer server(&loop,listen_addr,true);
	server.setOnConnCb([&loop](const TcpConnPtr& conn) {
		loop.runAfter(1, [conn] {
			conn->send("ready?\n");
		});
		loop.runEvery(3, [conn]() {
			conn->send("hello, world!\n");
		});
	});
	server.setThreadNum(0);
	server.start();
	loop.loop();
	return 0;
}
