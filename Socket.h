#pragma once
#include "Utils.h"
// Socket与Channel相比更偏向于管理sockfd本身的一些参数属性,是对socket本身的封装
// 持有sockfd，与sockfd有相同的生命期

namespace ryugu
{
	namespace net
	{
		class InetAddr;

		class Socket
		{
		public:
			explicit Socket(int sockfd)
				:sockfd_(sockfd) {}
			~Socket();
			int getFd() const { return sockfd_; }

			void bind(const InetAddr& localaddr);
			void listen();
			int accept(InetAddr* peeraddr);

			void setTcpNoDelay(bool enable);
			void setReuseAddr(bool enable);
			void setReusePort(bool enable);
			void setKeepAlive(bool enable);

		private:
			const int sockfd_;
		};
	}
}
