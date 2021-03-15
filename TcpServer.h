/*
TcpServer
实现了Tcp服务器
*/



#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "Callbacks.h"
#include "Utils.h"
#include "TcpConn.h"
#include "Net.h"
#include "InetAddr.h"
namespace ryugu
{
    class EventLoop;
    class Channel;
    class TcpServer;
    class EventLoopThreadPool;


    class TcpServer : noncopyable
    {
        public:
        TcpServer(EventLoop* _loop,const net::InetAddr&,bool);
        ~TcpServer(){}
        void setThreadNum(int num);
        void start();

        public:
        int bind(const std::string& host,unsigned short port,bool reusePort=false);
        int goListening();


        void setMessageCb(const MessageCallback& cb){readCb=cb;}
        void setOnConnCb(const OnConnCallback& cb){connCb=cb;}



        private:
        EventLoop* loop_;
        // fd->conn
		// 始终持有一个引用计数
        std::unordered_map<int,TcpConnPtr> connMap;

        std::unique_ptr<Channel> listenChannel;
        std::shared_ptr<EventLoopThreadPool> threadPool;

        net::InetAddr listenAddr;
        bool reusePort;

        // 回调函数
        // 连接建立
        OnConnCallback connCb;
        // 可读
        MessageCallback readCb;

        private:
        void handleAccept();
        void removeConnection(const TcpConnPtr&);
        void removeConnectionInLoop(const TcpConnPtr&);

    };
}
