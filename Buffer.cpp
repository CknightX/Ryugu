#include<unistd.h>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cassert>
#include "Buffer.h"
#include "Debug.h"
#include "SocketsOps.h"
namespace ryugu
{
	namespace net
	{
		Buffer::Buffer()
			: bg(0), ed(0)
		{
		}
		Buffer::~Buffer()
		{
		}
		void Buffer::append(const char* str)
		{
			auto len = strlen(str);
			writeIn(str, len);
		}
		void Buffer::append(const char* str, size_t len)
		{
			writeIn(str, len);
		}
		void Buffer::append(const std::string& str)
		{
			writeIn(str.c_str(), str.size());
		}
		void Buffer::writeIn(const char* str, size_t len)
		{
			auto remain = buf.capacity() - buf.size();
			// buf后端空间不够用，将数据前挪,如果仍然不够，vector会自动扩展
			if (bg != 0 && remain < len)
			{
				auto sz = size();
				std::copy(buf.begin() + bg, buf.begin() + ed, buf.begin());
				bg = 0;
				ed = bg + sz;
				buf.resize(sz);
			}
			auto insert_iter = std::back_inserter(buf);
			std::copy(str, str + len, insert_iter);
			ed += len;
		}
		void Buffer::writeIn(const std::string& str)
		{
			writeIn(str.data(), str.size());
		}
		ssize_t Buffer::writeInFromFd(int fd)
		{
			char tmp_buf[65536];
			int n = sockets::read(fd, tmp_buf, sizeof(tmp_buf));
			if (n < 0)
			{
				if (errno == EINTR)
					1; // Do nothing
				if (errno == EAGAIN)
					1; // Do nothing
			}
			else if (n == 0)
			{
				// client lost.
			}
			else
			{
				writeIn(tmp_buf, n);
			}
			return n;
		}
		ssize_t Buffer::readOutToFd(int fd)
		{
			size_t sended = 0;
			ssize_t nbytes = sockets::write(fd, getData(), size());
			if (nbytes > 0)
			{
				consume(nbytes);
			}
			// error
			else
			{
				LOG_ERROR("Buffer::readOutToFd");
			}
			return sended;
		}
		void Buffer::readOut(char* _buf, size_t len)
		{
			// 实际读出的长度
			auto rlen = std::min(len, size());
			std::copy(buf.begin() + bg, buf.begin() + bg + rlen, _buf);
			consume(rlen);
		}
		std::string Buffer::readOutAsString(size_t len)
		{
			auto rlen = std::min(len, size());
			std::string tmp(buf.begin() + bg, buf.begin() + bg + rlen);
			consume(rlen);
			return tmp;
		}
		const char* Buffer::find(const std::string& str) const
		{
			const char* res = nullptr;
			res = std::search(getPeek(), getEnd(),str.cbegin(),str.cend());
			return res;
		}
		void Buffer::consumeUntil(const char* pos)
		{
			assert(getPeek() <= pos);
			assert(pos <= getEnd());
			consume(pos - getPeek());
		}
	}

} // namespace ryugu
