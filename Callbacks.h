#pragma once
#include <memory>
#include <functional>

namespace ck
{
    class TcpConn;
    class EventHandler;

    using TcpConnPtr = std::shared_ptr<TcpConn>;
    using TcpCallBack = std::function<void(const TcpConnPtr &)>;

    using TimerCallback=std::function<void()>;

    using ThreadInitCallback=std::function<void(EventHandler*)>;

    using Functor=std::function<void()>;
}