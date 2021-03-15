#pragma once
#include <arpa/inet.h>
#include <string.h>
#include <string>

namespace ryugu
{
	namespace net
	{
		namespace sockets
		{
			void fillSockAddrInWithIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
			void fillSockAddrInWithIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);
			std::string getIpPortStrFromSockAddr(const struct sockaddr* addr);
			std::string getIpStrFromSockAddr(const struct sockaddr* addr);
			uint16_t getPortFromSockAddr(const struct sockaddr* addr);

			struct sockaddr_in getLocalAddr(int sockfd);
			struct sockaddr_in getPeerAddr(int sockfd);

			int createNonblockingSocket(sa_family_t family);
			int connect(int sockfd, const struct sockaddr* addr);
			void bind(int sockfd, const struct sockaddr* addr);
			void listen(int sockfd);
			int accept(int sockfd, struct sockaddr* addr);
			void close(int sockfd);

			inline void memZero(void* p, size_t n) { memset(p, 0, n); }

			// cast
			const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr_in);
			struct sockaddr* sockaddr_cast(struct sockaddr_in* addr_in);
			const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr_in);
			const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
			const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);
		}
	}
}
