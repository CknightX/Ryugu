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

        void setConnRead(const TcpCallBack& cb){readcb=cb;}



        private:
        EventHandlers* handlers;
        EventHandler* handler;

        Ipv4Addr addr;
        Channel* listenChannel;

        // readcb即使用者设定的可读回调函数
        TcpCallBack readcb;

        std::function<TcpConnPtr()> createCB;

        private:
        // accept的回调函数，用来绑定在listenChannel的read事件上
        void handleAccept();






    };
}