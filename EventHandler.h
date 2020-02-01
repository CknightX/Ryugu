/*
EventHandler
*/
#pragma once
#include <memory>
#include "poller.h"
#include "debug.h"
#include "TimerId.h"
#include "Timestamp.h"
#include "TimerQueue.h"
#include "Timer.h"

namespace ck
{

class Channel;


// 事件处理器
class EventHandler 
{
    public:
    EventHandler();
    ~EventHandler();

    void loopOnce(int waitMs);
    void loop();
    void stop(){isStop=true;}

    // 定时器任务
    TimerId runAt(Timestamp time,TimerCallback cb);
    TimerId runAfter(double delay,TimerCallback cb);
    TimerId runEvery(double interval,TimerCallback cb);



    public:
    Poller* poller;

    private:
    bool isStop;
    std::unique_ptr<TimerQueue> timerQueue;



    

};
}