/*
TcpServer
实现了Tcp服务器
*/



#pragma once

#include "utils.h"
#include "conn.h"
#include "net.h"
#include <memory>


namespace ck
{
    class EventHandler;
    class EventHandlers;
    class Channel;
    class TcpServer;

    using TcpServerPtr=std::shared_ptr<TcpServer>;
    class TcpServer : noncopyable
    {
        public:
        TcpServer(EventHandler* _handler);
        ~TcpServer(){}

        public:
        // 绑定端口，同时设置listenChannel的read事件回调函数为handleAccept
        int bind(const std::string& host,unsigned short port,bool reusePort=false);
        static TcpServerPtr startServer(EventHandler* _handler,const std::string& host,unsigned short port,bool reusePort=false);

        void setReadCb(const TcpCallBack& cb){readCb=cb;}
        void setConnCb(const TcpCallBack& cb){connCb=cb;}



        private:
        EventHandler* handler;

        net::Ipv4Addr addr;
        Channel* listenChannel;

        // 连接建立
        TcpCallBack connCb;
        // 可读
        TcpCallBack readCb;

        std::function<TcpConnPtr()> createCB;

        private:
        void handleAccept();






    };
}