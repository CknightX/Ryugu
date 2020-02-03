#include "TcpServer.h"
#include "EventHandler.h"
#include "debug.h"
#include "channel.h"
#include "EventHandlerThreadPool.h"
#include <cstring>
namespace ck
{
extern Epoll *globalEpoll;

TcpServer::TcpServer(EventHandler *_handler)
    : handler(_handler), listenChannel(nullptr), createCB([] { return TcpConnPtr(new TcpConn); }),
      threadPool(new EventHandlerThreadPool(handler, "test"))
{
}

int TcpServer::bind(const std::string &host, unsigned short port, bool reusePort)
{
    int r;
    addr = net::Ipv4Addr(host, port);
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    r = ::bind(fd, (sockaddr *)&addr.getAddr(), sizeof(sockaddr));
    if (r)
    {
        ::close(fd);
        LOG_ERROR("bind");
        return errno;
    }
    r = listen(fd, 20);

    listenChannel = new Channel(handler, fd, cstReadEvent);
    listenChannel->setReadCB([this] { handleAccept(); });
    return r;
}

TcpServerPtr TcpServer::startServer(EventHandler *_handler, const std::string &host, unsigned short port, bool reusePort)
{
    TcpServerPtr p(new TcpServer(_handler));
    int r = p->bind(host, port, reusePort);
    if (r)
    {
        LOG_ERROR("bind");
    }
    p->setThreadNum(0);
    p->threadPool->start();

    return r == 0 ? p : nullptr;
}

void TcpServer::handleAccept()
{
    EventHandler *subHandler = threadPool->getOneHandler();
    std::cout<<"use thread handler:"<<subHandler->threadId<<std::endl;
    LOG("handleAccept");
    sockaddr_in cliaddr;
    socklen_t cliaddrLen = sizeof(cliaddr);
    int lfd = listenChannel->getFd();
    int cfd;
    if (lfd > 0)
    {
        // 之前阻塞在这里
        while ((cfd = accept(lfd, (sockaddr *)&cliaddr, &cliaddrLen)) >= 0)
        {
            sockaddr_in peer, local;
            socklen_t addrLen = sizeof(peer);
            int r = getpeername(cfd, (sockaddr *)&peer, &addrLen);
            if (r < 0)
            {
                LOG_ERROR("get peer name failed");
                continue;
            }
            r = getsockname(cfd, (sockaddr *)&local, &addrLen);
            if (r < 0)
            {
                LOG_ERROR("getsockname failed");
            }

            // 有新的客户端连接
            TcpConnPtr con(new TcpConn);

            if (connCb)
            {
                con->setConnCb(connCb);
            }

            // 将fd与TcpConnection关联起来
            con->attach(subHandler, cfd, local, peer);

            // readcb
            if (readCb)
            {
                con->setReadCb(readCb);
            }
        }
    }
}

void TcpServer::setThreadNum(int num)
{
    threadPool->setThreadNum(num);
}
} // namespace ck

// namespace ck