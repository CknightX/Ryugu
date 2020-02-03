#include "EventHandlerThread.h"
#include "EventHandler.h"

namespace ck
{
    EventHandlerThread::EventHandlerThread(const std::string& name)
    {

    }
    EventHandler* EventHandlerThread::start()
    {
        threadPtr=std::make_shared<std::thread>(std::bind(&EventHandlerThread::threadFunc,this));
        EventHandler* _handler=nullptr;

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
    void EventHandlerThread::threadFunc()
    {
        EventHandler localHandler;

        {
            std::lock_guard<std::mutex> mux(mutex);
            handler=&localHandler;
            condition.notify_one();
        }

        MARK
        localHandler.loop();

        // ???
        std::lock_guard<std::mutex> mux(mutex);
        handler=nullptr;



    }

    EventHandlerThread::~EventHandlerThread()
    {

    }
}