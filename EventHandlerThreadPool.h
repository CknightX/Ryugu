#pragma once
#include <string>
#include <vector>
#include "Utils.h"
#include "Callbacks.h"

namespace ck
{
class EventHandler;
class EventHandlerThread;

// one loop per thread
class EventHandlerThreadPool : noncopyable
{
    public:
    EventHandlerThreadPool(EventHandler* _baseHandler,const std::string& _name);
    ~EventHandlerThreadPool();
    void setThreadNum(int nums){numThreads=nums;}
    void start();

    EventHandler* getOneHandler();
    std::vector<EventHandler*> getAllHandler();

    bool isStarted() const {return started;}
    const std::string& getName() const {return name;}

    private:
    EventHandler* baseHandler;
    std::string name;
    bool started;
    int numThreads;
    // 下一次选择的handler序号
    int next;
    std::vector<std::unique_ptr<EventHandlerThread>> threads;
    std::vector<EventHandler*> handlers;
};
}