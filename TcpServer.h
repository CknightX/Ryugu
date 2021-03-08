/*
TcpServer
实现了Tcp服务器
*/



#pragma once

#include <vector>
#include <memory>
#include "Callbacks.h"
#include "Utils.h"
#include "TcpConn.h"
#include "Net.h"
namespace ck
{
    class EventLoop;
    class Channel;
    class TcpServer;
    class EventLoopThreadPool;


    class TcpServer : noncopyable
    {
        public:
        TcpServer(EventLoop* _loop,const net::Ipv4Addr&,bool);
        ~TcpServer(){}
        void setThreadNum(int num);
        void start();

        public:
        int bind(const std::string& host,unsigned short port,bool reusePort=false);
        int goListening();


        void setMessageCb(const MessageCallback& cb){readCb=cb;}
        void setOnConnCb(const OnConnCallback& cb){connCb=cb;}



        private:
        EventLoop* loop;
        std::vector<TcpConnPtr> conns;

        std::unique_ptr<Channel> listenChannel;
        std::shared_ptr<EventLoopThreadPool> threadPool;

        net::Ipv4Addr listenAddr;
        bool reusePort;

        // 回调函数
        // 连接建立
        OnConnCallback connCb;
        // 可读
        MessageCallback readCb;

        private:
        void handleAccept();






    };
}
