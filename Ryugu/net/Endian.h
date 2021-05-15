#pragma once
#include <endian.h>
#include <stdint.h>
namespace ryugu
{
	namespace net
	{
		namespace sockets
		{
			// Host to Network
			inline uint64_t h2n64(uint64_t host64)
			{
				return htobe64(host64);
			}
			inline uint32_t h2n32(uint32_t host32)
			{
				return htobe32(host32);
			}
			inline uint16_t h2n16(uint16_t host16)
			{
				return htobe16(host16);
			}
			// Network to Host
			inline uint64_t n2h64(uint64_t net64)
			{
				return be64toh(net64);
			}
			inline uint32_t n2h32(uint32_t net32)
			{
				return be32toh(net32);
			}
			inline uint16_t n2h16(uint16_t net16)
			{
				return be16toh(net16);
			}
		}
	}
}
