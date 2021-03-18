#include <unistd.h>
#include "SocketsOps.h"
#include "Endian.h"
#include "Debug.h"
#include "Utils.h"
namespace ryugu
{
	namespace net
	{
		namespace sockets
		{
			// cast
			const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr_in)
			{
				// Unsafe ,use implicit_cast instead of it.
				return static_cast<const struct sockaddr*>(static_cast<const void*>(addr_in));
			}
			struct sockaddr* sockaddr_cast(struct sockaddr_in* addr_in)
			{
				// Unsafe ,use implicit_cast instead of it.
				return static_cast<struct sockaddr*>(static_cast<void*>(addr_in));
			}
			const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr_in)
			{
				return static_cast<const struct sockaddr*>(static_cast<const void*>(addr_in));
			}
			const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr)
			{
				return static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr));
			}
			const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr)
			{
				return static_cast<const struct sockaddr_in6*>(static_cast<const void*>(addr));
			}
			int createNonblockingSocket(sa_family_t family)
			{
				int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
				if (sockfd < 0)
				{
					LOG_ERROR("sockets::createNonblockingSocket");
				}
				return sockfd;
			}

			void bind(int sockfd, const struct sockaddr* addr)
			{
				int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
				if (ret < 0)
				{
					LOG_ERROR("sockets::bind");
				}
			}
			void listen(int sockfd)
			{
				int ret = ::listen(sockfd, SOMAXCONN);
				if (ret < 0)
				{
					LOG_ERROR("sockets::listen");
				}
			}
			int accept(int sockfd, struct sockaddr* addr)
			{
				auto addr_len = static_cast<socklen_t> (sizeof(struct sockaddr_in));
				int connfd = ::accept(sockfd, addr, &addr_len);
				if (connfd < 0)
				{
					LOG_ERROR("sockets::accept");
				}
				return connfd;
			}
			int connect(int sockfd, const struct sockaddr* addr)
			{
				auto addr_len = static_cast<socklen_t> (sizeof(struct sockaddr_in));
				return ::connect(sockfd, addr, addr_len);
			}
			void close(int sockfd)
			{
				if (::close(sockfd) < 0)
				{
					LOG_ERROR("sockets::close");
				}
			}
			void shutdownWrite(int sockfd)
			{
				if (::shutdown(sockfd, SHUT_WR) < 0)
				{
					LOG_ERROR("sockets::shutdownWrite");
				}
			}
			void fillSockAddrInWithIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
			{
				addr->sin_family = AF_INET;
				addr->sin_port = h2n16(port);
				if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
				{
					LOG_ERROR("sockets::fillSockAddrInWithIpPort");
				}
			}
			void fillSockAddrInWithIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr)
			{
				addr->sin6_family = AF_INET6;
				addr->sin6_port = h2n16(port);
				if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
				{
					LOG_ERROR("sockets::fillSockAddrIn6WithIpPort");
				}
			}
			std::string getIpStrFromSockAddr(const struct sockaddr* addr)
			{
				char buf[64];
				auto sz = static_cast<socklen_t>(sizeof buf);
				if (addr->sa_family == AF_INET)
				{
					auto addr4 = sockaddr_in_cast(addr);
					::inet_ntop(AF_INET, &addr4->sin_addr, buf, sz);

				}
				else if (addr->sa_family == AF_INET6)
				{
					auto addr6 = sockaddr_in6_cast(addr);
					::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, sz);
				}
				return buf;
			}
			std::string getIpPortStrFromSockAddr(const struct sockaddr* addr)
			{
				std::string res;
				res.append(getIpStrFromSockAddr(addr));
				res.append(std::to_string(getPortFromSockAddr(addr)));
				return res;
			}
			uint16_t getPortFromSockAddr(const struct sockaddr* addr)
			{
				auto addr_in = sockets::sockaddr_in_cast(addr);
				return n2h16(addr_in->sin_port);
			}
			struct sockaddr_in getLocalAddr(int sockfd)
			{
				struct sockaddr_in localaddr;
				memZero(&localaddr, sizeof localaddr);
				auto addrlen = static_cast<socklen_t>(sizeof localaddr);
				if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
				{
					LOG_ERROR("sockets::getLocalAddr");
				}
				return localaddr;

			}
			struct sockaddr_in getPeerAddr(int sockfd)
			{
				struct sockaddr_in peeraddr;
				memZero(&peeraddr, sizeof peeraddr);
				auto addrlen = static_cast<socklen_t>(sizeof peeraddr);
				if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
				{
					LOG_ERROR("sockets::getLocalAddr");
				}
				return peeraddr;
			}

		}
	}
}