#pragma once

#include "../TcpServer.h"
#include "../Buffer.h"
#include "../Noncopyable.h"
#include "../TcpConn.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <string>
#include<iostream>

class WebServer : ryugu::noncopyable
{
public:
	using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

	WebServer(ryugu::net::EventLoop* loop, const ryugu::net::InetAddr& listenAddr, const std::string& name, bool reUsePort = true);
	ryugu::net::EventLoop* getLoop() const { return server_.getLoop(); }
	void setHttpCB(const HttpCallback& cb) { httpCB_ = cb; }
	void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }
	void start() { server_.start(); }
private:
	ryugu::net::TcpServer server_;
	HttpCallback httpCB_;

	void onConnection(const ryugu::net::TcpConnPtr& conn);
	void onMessage(const ryugu::net::TcpConnPtr& conn);
	void onRequest(const ryugu::net::TcpConnPtr&, const HttpRequest&);
};
WebServer::WebServer(ryugu::net::EventLoop* loop, const ryugu::net::InetAddr& listenAddr, const std::string& name, bool reUsePort)
	:server_(loop,listenAddr,reUsePort)
{
	server_.setMessageCb([this](const ryugu::net::TcpConnPtr& conn) {
		this->onMessage(conn);
	});
	server_.setOnConnCb([this](const ryugu::net::TcpConnPtr& conn) {
		this->onConnection(conn);
	});
}
void WebServer::onConnection(const ryugu::net::TcpConnPtr& conn)
{
	if (conn->isConnected())
	{
		conn->setContext(std::make_shared<HttpContext>());
	}
}
void WebServer::onMessage(const ryugu::net::TcpConnPtr& conn)
{
	/*/
	std::string header = "";
	header += "HTTP/1.1 200 OK\r\n";
	header += "Content-type: text/html\r\n";
	header += "Content-length: 1\r\n";
	header += "Connection: keep-alive\r\n";
	header += "\r\n";
	header += "c";
	conn->send(header);
	*/
	auto context = std::static_pointer_cast<HttpContext>(conn->getContext());
	// 解析过程中出现错误
	if (!context->parseRequest(conn->getBuffer(),ryugu::Timestamp::getNow()))
	{
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}
	// 接收完整才开始处理
	if (context->gotAll())
	{
		onRequest(conn, context->getRequest());
		context->reset();
	}
}
// 返回响应报文
void WebServer::onRequest(const ryugu::net::TcpConnPtr& conn, const HttpRequest& req)
{
	const std::string& header_connection = req.getHeader("Conneciton");
	// 是否关闭连接
	bool close = header_connection == "close" ||
		(req.getVersion() == HttpRequest::Version::kHttp10 && header_connection != "Keep-Alive");
	HttpResponse response(close);
	// 解析body
	httpCB_(req, &response);
	ryugu::net::Buffer buffer;
	response.appendToBuffer(&buffer);
	auto strResponse = buffer.readOutAsString(buffer.size());
	conn->send(strResponse);
	if (response.isClose())
	{
		conn->shutdown();
	}
}
