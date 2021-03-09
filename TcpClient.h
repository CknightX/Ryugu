#pragma once
#include "Callbacks.h"
#include "TcpConn.h"

namespace ryugu
{
    class EventHandler;
    class TcpClient
    {
        public:
        // 发起连接
        void connect();

        // 断开连接
        void disconnect();

        private:
        bool isConnected;
        
        EventHandler* handler;

        TcpCallBack connectionCb;
        TcpCallBack messageCb;

        TcpConnPtr connection;
        



    };

}