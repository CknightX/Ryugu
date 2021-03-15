#pragma once
#include "Utils.h"
// Socket��Channel��ȸ�ƫ���ڹ���sockfd�����һЩ��������,�Ƕ�socket����ķ�װ
// ����sockfd����sockfd����ͬ��������

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
