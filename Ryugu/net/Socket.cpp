#include <netinet/tcp.h>
#include <netinet/in.h>
#include "Ryugu/net/Socket.h"
#include "Ryugu/net/InetAddr.h"
#include "Ryugu/net/SocketsOps.h"
#include "Ryugu/base/Utils.h"
#include "Ryugu/base/log/Logging.h"

namespace ryugu
{
	namespace net
	{
		void Socket::bind(const InetAddr& addr)
		{
			sockets::bind(sockfd_, addr.getSockAddr());
		}
		void Socket::listen()
		{
			sockets::listen(sockfd_);
		}
		int Socket::accept(InetAddr* peeraddr)
		{
			struct sockaddr_in addr;
			memZero(&addr, sizeof addr);
			int connfd = sockets::accept(sockfd_, reinterpret_cast<sockaddr*>(&addr));
			if (connfd >= 0)
			{
				peeraddr->setSockAddr(addr);
			}
			return connfd;
		}
		void Socket::setTcpNoDelay(bool enable)
		{
			int optval = enable ? 1 : 0;
			::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
				&optval, static_cast<socklen_t>(sizeof optval));
		}
		void Socket::setReuseAddr(bool enable)
		{
			int optval = enable ? 1 : 0;
			::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
				&optval, static_cast<socklen_t>(sizeof(optval)));
		}
		void Socket::setReusePort(bool enable)
		{
		#ifdef SO_REUSEPORT
			int optval = enable ? 1 : 0;
			int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
				&optval, static_cast<socklen_t>(sizeof optval));
			if (ret < 0 && enable)
			{
				LOG_ERROR << "SO_REUSEPORT failed.";
			}
		#else
			if (enable)
			{
				LOG_ERROR << "SO_REUSEPORT is not supported.";
			}
		#endif
		}
		void Socket::setKeepAlive(bool enable)
		{
			int optval = enable ? 1 : 0;
			::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
				&optval, static_cast<socklen_t>(sizeof optval));
		}
		void Socket::shutdownWrite()
		{
			sockets::shutdownWrite(sockfd_);
		}
		Socket::~Socket()
		{
			sockets::close(sockfd_);
		}

	}
}

