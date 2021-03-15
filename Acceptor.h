#pragma once
#include <functional>
#include <memory>
namespace ryugu
{
	namespace net
	{
		class EventLoop;
		class InetAddr;
		class Socket;
		class Channel;

		class Acceptor
		{
		public:
			using NewConnCallback=std::function<void(int sockfd,const InetAddr&)>;
			Acceptor(EventLoop* loop, const InetAddr& listenAddr, bool reusePort);
			~Acceptor();

			void setNewConnCallback(const NewConnCallback& newConnCb) { newConnCb_ = newConnCb; }
			void listen();
			bool isListening() const { return listening_; }


		private:
			void handleRead();

			EventLoop* loop_;
			std::unique_ptr<Socket> acceptSocket_;
			std::unique_ptr<Channel> acceptChannel_;

			NewConnCallback newConnCb_;
			bool listening_;
		};

	}

}
