/*
TcpServer
*/



#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "Callbacks.h"
#include "Utils.h"
#include "Net.h"
#include "Noncopyable.h"
namespace ryugu
{
	namespace net
	{
		class EventLoop;
		class Channel;
		class TcpServer;
		class Acceptor;
		class EventLoopThreadPool;
		class InetAddr;


		class TcpServer : noncopyable
		{
		public:
			TcpServer(EventLoop* _loop, const InetAddr&, bool);
			~TcpServer();
			void setThreadNum(int num);
			void start();
			EventLoop* getLoop() const { return loop_; }

		public:

			void setMessageCb(const MessageCallback& cb) { messageCb = cb; }
			void setOnConnCb(const OnConnCallback& cb) { connCb = cb; }

		private:
			// for Acceptor
			void newConnection(int sockfd, const InetAddr& peerAddr);

			void removeConnection(const TcpConnPtr&);
			void removeConnectionInLoop(const TcpConnPtr&);


		private:
			EventLoop* loop_;
			// fd->conn
			// 始终持有一个引用计数
			std::unordered_map<int, TcpConnPtr> connMap_;
			std::shared_ptr<EventLoopThreadPool> threadPool_;
			std::unique_ptr<Acceptor> acceptor_;



			// 回调函数
			// 连接建立
			OnConnCallback connCb;
			// 可读
			MessageCallback messageCb;


		};
	}
}
