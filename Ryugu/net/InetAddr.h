#pragma once
#include <netinet/in.h>
#include <string>
#include "Ryugu/net/SocketsOps.h"

namespace ryugu
{
	namespace net
	{
		class InetAddr
		{
		public:
			explicit InetAddr(uint16_t port=0, bool lookbackOnly = false, bool ipv6 = false);
			explicit InetAddr(const struct sockaddr_in& addr) :addr_(addr) {}
			explicit InetAddr(const struct sockaddr_in6& addr6) :addr6_(addr6) {}
			InetAddr(std::string ip, uint16_t port, bool ipv6 = false);

			sa_family_t family() const { return addr_.sin_family; }
			std::string getIpStr() const;
			std::string getIpPortStr() const;
			uint16_t getPort() const;

			// addr_和addr6_在一个匿名union中，所以返回哪个都是可以的
			const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr_); }
			const struct sockaddr_in* getSockAddrIn() const { return &addr_; }
			uint32_t getIpNetEndian() const;
			uint16_t getPortNetEndian() const { return addr_.sin_port; }

			void setSockAddr(const struct sockaddr_in& addr) { addr_ = addr; }

		private:
			union
			{
				struct sockaddr_in addr_;
				struct sockaddr_in6 addr6_;
			};
		};
	}
}