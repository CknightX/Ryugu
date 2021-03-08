#include "TcpServer.h"
#include "EventLoop.h"
#include "Net.h"
#include "Debug.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <cstring>
namespace ck
{

    TcpServer::TcpServer(EventLoop *_loop,const net::Ipv4Addr& _listenAddr, bool reusePort)
        :loop(_loop),
        listenAddr(_listenAddr),
        reusePort(reusePort),
        listenChannel(nullptr),
        threadPool(new EventLoopThreadPool(loop, "test"))
    {
    }

    void TcpServer::start()
    {
        int r=goListening();
        setThreadNum(1);
        threadPool->start();
    }

    int TcpServer::goListening()
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        int one=1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
        {
            close(fd);
            return -1;
        }

        int r = ::bind(fd, (sockaddr *)&listenAddr.getAddr(), sizeof(sockaddr));
        if (r)
        {
            ::close(fd);
            LOG_ERROR("bind");
            LOG("errno:%d",errno);
            return errno;
        }
        r = listen(fd, 20);

        listenChannel.reset(new Channel(loop, fd));
        LOG("listenChannel created.");
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
                // 构造TcpConn
                TcpConnPtr conn(new TcpConn(subLoop,cfd,local,peer));
                // 回调
                conn->setConnCb(connCb);
                conn->setReadCb(readCb);

                // IO线程调用establish->channel纳入poller中开始事件循环
                subLoop->runInLoop([conn]{
                    conn->connectEstablished();
                });

                // 加入连接池
                conns.emplace_back(conn);
            }
        }
    }
    void TcpServer::setThreadNum(int num)
    {
        threadPool->setThreadNum(num);
    }
} // namespace ck

// namespace ck
