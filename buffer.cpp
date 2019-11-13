#include "buffer.h"
#include "debug.h"
#include<unistd.h>
#include<iostream>
#include <algorithm>
namespace ck
{
Buffer::Buffer()
    : bg(0), ed(0)
{
}
Buffer::~Buffer()
{
}

void Buffer::writeIn(const char *str, size_t len)
{
    auto remain = buf.capacity() - buf.size();
    // buf后端空间不够用，将数据前挪,如果仍然不够，vector会自动扩展
    if (bg != 0 && remain < len)
    {
        auto sz = size();
        LOG("size=%d", sz);
        std::copy(buf.begin() + bg, buf.begin() + ed, buf.begin());
        bg = 0;
        ed = bg + sz;
        buf.resize(sz);
    }

    for (auto i = 0; i < len; ++i)
        buf.push_back(*(str++));
    ed += len;
}

void Buffer::writeIn(const std::string &str)
{
    writeIn(str.data(), str.size());
}
void Buffer::readOut(char *_buf,size_t len)
{
    // 实际读出的长度
    auto rlen = std::min(len, size());
    std::copy(buf.begin() + bg, buf.begin() + bg + rlen, _buf);
    bg += rlen;
}
std::string Buffer::readOutAsString(size_t len)
{
    auto rlen = std::min(len, size());
    std::string tmp(buf.begin() + bg, buf.begin() + bg + rlen);
    bg += rlen;
    return tmp;
}

void Buffer::getDataFromFd(int fd)
{
    static char _buf[256];
    int n;
    while(1)
    {
        n=read(fd,_buf,256);
        if (n<0)
        {
            // 被中断打断
            if (errno==EINTR)
                continue;

            // LT模式读取完毕
            if (errno==EAGAIN)
                break;
        }

        writeIn(_buf,n);
    }
}

} // namespace ck