#include "EventHandlerThreadPool.h"
#include "EventHandlerThread.h"
#include "Debug.h"
namespace ck
{
    EventHandlerThreadPool::EventHandlerThreadPool(EventHandler* _baseHandler,const std::string& _name)
    :baseHandler(_baseHandler),
    name(_name),
    started(false),
    numThreads(0),
    next(0)
    {

    }
    EventHandlerThreadPool::~EventHandlerThreadPool()
    {

    }

    void EventHandlerThreadPool::start()
    {
        if (started)
            return;

        started=true;

        for(int i=0;i<numThreads;++i)
        {
            LOG("The number of threads:%d",numThreads);
            std::string tname=name+std::to_string(i);
            EventHandlerThread* t=new EventHandlerThread(tname);
            threads.push_back(std::unique_ptr<EventHandlerThread>(t));
            handlers.push_back(t->start());
        }
    }

    EventHandler* EventHandlerThreadPool::getOneHandler()
    {
        EventHandler* handler=baseHandler;

        if (!handlers.empty())
        {
            handler=handlers[next++];
            if (static_cast<size_t>(next)>=handlers.size())
                next=0;
        }
        return handler;
    }

    std::vector<EventHandler*> EventHandlerThreadPool::getAllHandler()
    {

        if (handlers.empty())
        {
            return std::vector<EventHandler*>{baseHandler};
        }
        else
        {
            return handlers;
        }
    }
}