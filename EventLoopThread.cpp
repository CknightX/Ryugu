#include "EventLoopThread.h"
#include "EventLoop.h"

namespace ck
{
    EventLoopThread::EventLoopThread(const std::string& name)
    {

    }
    EventLoop* EventLoopThread::start()
    {
        threadPtr=std::make_shared<std::thread>(std::bind(&EventLoopThread::threadFunc,this));
        EventLoop* _handler=nullptr;

        {
            std::unique_lock<std::mutex> mux(mutex);
            while(handler==nullptr)
            {
                condition.wait(mux);
            }
            _handler=handler;

        }
        LOG("create one thread.");
        return _handler;
    }
    void EventLoopThread::threadFunc()
    {
        EventLoop localLoop;

        // 局部作用域，为了尽快释放lock_guard
        {
            std::lock_guard<std::mutex> mux(mutex);
            handler=&localLoop;
            condition.notify_one();
        }

        localLoop.loop();

        // ???
        std::lock_guard<std::mutex> mux(mutex);
        handler=nullptr;

    }

    EventLoopThread::~EventLoopThread()
    {

    }
}
