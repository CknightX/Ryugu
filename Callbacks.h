#pragma once
#include <memory>
#include <functional>

namespace ck
{
    class TcpConn;
    class EventLoop;

    using TcpConnPtr = std::shared_ptr<TcpConn>;
    using TcpCallBack = std::function<void(const TcpConnPtr &)>;

    using TimerCallback=std::function<void()>;

    using ThreadInitCallback=std::function<void(EventLoop*)>;

    using Functor=std::function<void()>;

    // for TcpServer
    using OnConnCallback=std::function<void(const TcpConnPtr&)>;
    using MessageCallback=std::function<void(const TcpConnPtr&)>;


}
