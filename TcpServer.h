/*
TcpServer
实现了Tcp服务器
*/



#pragma once

#include "Utils.h"
#include "TcpConn.h"
#include "Net.h"
#include <memory>


namespace ck
{
    class EventLoop;
    class Channel;
    class TcpServer;
    class EventLoopThreadPool;

    using TcpServerPtr=std::shared_ptr<TcpServer>;
    class TcpServer : noncopyable
    {
        public:
        TcpServer(EventLoop* _handler);
        ~TcpServer(){}

        public:
        // 绑定端口，同时设置listenChannel的read事件回调函数为handleAccept
        int bind(const std::string& host,unsigned short port,bool reusePort=false);
        static TcpServerPtr startServer(EventLoop* _handler,const std::string& host,unsigned short port,bool reusePort=false);

        void setThreadNum(int num);

        void setReadCb(const TcpCallBack& cb){readCb=cb;}
        void setConnCb(const TcpCallBack& cb){connCb=cb;}



        private:
        EventLoop* loop;
        std::shared_ptr<EventLoopThreadPool> threadPool;

        net::Ipv4Addr addr;
        Channel* listenChannel;

        // 回调函数
        // 连接建立
        TcpCallBack connCb;
        // 可读
        TcpCallBack readCb;

        std::function<TcpConnPtr()> createCB;

        private:
        void handleAccept();






    };
}
