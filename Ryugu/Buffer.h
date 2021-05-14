/**
 * Buffer
 */
#pragma once
#include <cstddef>
#include <vector>
#include <string>

namespace ryugu
{
	namespace net
	{
		class Buffer
		{
		public:

			Buffer();
			~Buffer();

			void writeIn(const char* str, size_t len);
			void writeIn(const std::string& str);

			void append(const char* str, size_t len);
			void append(const char* str);   // unsafe
			void append(const std::string& str);
			ssize_t writeInFromFd(int fd);
			ssize_t readOutToFd(int fd);
			void readOut(char* _buf, size_t len);
			// 以string类型读出
			std::string readOutAsString(size_t len);
			void consume(size_t len) { bg += len; }
			void consumeUntil(const char* pos);
			size_t size() const { return ed - bg; }
			// 获取字符串指针
			const char* getData()const { return buf.data(); }
			const char* getPeek() const { return buf.data() + bg; }
			const char* getEnd() const { return buf.data() + ed; }
			const char* find(const std::string& str) const;
			bool empty() const { return size() == static_cast<size_t>(0); }

		private:
			// [bg,ed) 所指示区域为有效数据
			size_t bg, ed;
			std::vector<char> buf;

		};
	}

}