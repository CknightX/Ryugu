#include <assert.h>
#include <string.h>
#include "Ryugu/net/InetAddr.h"
#include "Ryugu/net/Endian.h"
#include "Ryugu/net/SocketsOps.h"
namespace
{
	const in_addr_t kInaddrAny = INADDR_ANY;
	const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
}
namespace ryugu
{
	namespace net
	{
		InetAddr::InetAddr(uint16_t port, bool loopbackOnly, bool ipv6)
		{
			if (ipv6)
			{
				memset(&addr6_, 0, sizeof(addr6_));
				addr6_.sin6_family = AF_INET6;
				in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
				addr6_.sin6_addr = ip;
				addr6_.sin6_port = sockets::h2n16(port);
			}
			else
			{
				memset(&addr_, 0, sizeof(addr_));
				addr_.sin_family = AF_INET;
				in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
				addr_.sin_addr.s_addr = sockets::h2n32(ip);
				addr_.sin_port = sockets::h2n16(port);
			}
		}
		InetAddr::InetAddr(std::string ip, uint16_t port, bool ipv6)
		{
			if (ipv6)
			{
				memset(&addr6_,0, sizeof(addr6_));
				sockets::fillSockAddrInWithIpPort(ip.c_str(), port, &addr6_);
			}
			else
			{
				memset(&addr_, 0, sizeof(addr_));
				sockets::fillSockAddrInWithIpPort(ip.c_str(), port, &addr_);
			}
		}
		std::string InetAddr::getIpPortStr() const
		{
			return sockets::getIpPortStrFromSockAddr(getSockAddr());
		}
		std::string InetAddr::getIpStr() const
		{
			return sockets::getIpStrFromSockAddr(getSockAddr());
		}
		uint32_t InetAddr::getIpNetEndian() const
		{
			assert(family() == AF_INET);
			return addr_.sin_addr.s_addr;
		}
		uint16_t InetAddr::getPort() const
		{
			return sockets::getPortFromSockAddr(getSockAddr());
			return sockets::n2h16(getPortNetEndian());
		}
	}
}