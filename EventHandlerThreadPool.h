#pragma once
#include <string>
#include <vector>
#include "utils.h"
#include "Callbacks.h"

namespace ck
{
class EventHandler;
class EventHandlerThread;

// one loop per thread
class EventHandlerThreadPool : noncopyable
{
    public:
    EventHandlerThreadPool(EventHandler* _handler,const std::string& _name);
    ~EventHandlerThreadPool();
    void setThreadNum(int nums){numThreads=nums;}
    void start(const ThreadInitCallback& cb=ThreadInitCallback());

    EventHandler* getOneHandler()const;

    std::vector<EventHandler*> getAllHandler()const;

    bool isStarted() const {return started;}
    const std::string& getName() const {return name;}

    private:
    EventHandler* baseHandler;
    std::string name;
    bool started;
    int numThreads;
    int next;
    std::vector<std::unique_ptr<EventHandlerThread>> threads;
    std::vector<EventHandler*> loops;
};
}