#include "TcpServer.h"
#include "EventLoop.h"
#include "Net.h"
#include "Debug.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <cstring>
namespace ck
{

    TcpServer::TcpServer(EventLoop *_loop,const net::Ipv4Addr& listenAddr, bool reusePort)
        :loop(_loop),
        listenAddr(listenAddr),
        reusePort(reusePort),
        listenChannel(nullptr),
        threadPool(new EventLoopThreadPool(loop, "test"))
    {
    }

    void TcpServer::start()
    {
        int r=goListening();
        if (r)
        {
            LOG_ERROR("bind");
        }
        setThreadNum(10);
        threadPool->start();
    }

    int TcpServer::goListening()
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        int r = ::bind(fd, (sockaddr *)&listenAddr.getAddr(), sizeof(sockaddr));
        if (r)
        {
            ::close(fd);
            LOG_ERROR("bind");
            return errno;
        }
        r = listen(fd, 20);

        listenChannel.reset(new Channel(loop, fd, cstReadEvent));
        listenChannel->setReadCB([this] { handleAccept(); });
        return r;
    }

    // client端connect
    void TcpServer::handleAccept()
    {
        EventLoop *subLoop = threadPool->getOneLoop();
        //std::cout<<"use thread loop:"<<subLoop->threadId<<std::endl;
        LOG("handleAccept");
        sockaddr_in cliaddr;
        socklen_t cliaddrLen = sizeof(cliaddr);
        int lfd = listenChannel->getFd();
        int cfd;
        if (lfd > 0)
        {
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
                // 创建一个Connection
                TcpConnPtr con(new TcpConn);
                // 连接时的回调函数
                if (connCb)
                {
                    con->setConnCb(connCb);
                }
                // 将fd与TcpConnection关联起来,创建Channel
                con->attach(subLoop, cfd, local, peer);

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
