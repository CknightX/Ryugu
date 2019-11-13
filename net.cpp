#include "net.h"
#include <cstring>
namespace ck
{

Ipv4Addr::Ipv4Addr(const std::string& host, unsigned short port) 
{
    memset(&addr,0,sizeof addr);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=INADDR_ANY;
}

int setNonBlocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option); //设置为非阻塞套接字
    return old_option;
}

} // namespace ck