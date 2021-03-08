/*
EventLoop
*/
#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include "Poller.h"
#include "Debug.h"
#include "TimerId.h"
#include "Timestamp.h"
#include "TimerQueue.h"
#include "Timer.h"

namespace ck
{

class Channel;


// 事件处理器
class EventLoop 
{
    public:
    EventLoop();
    ~EventLoop();

    void loopOnce(int waitMs);
    void loop();
    void stop(){isStop=true;}

    // 定时器任务
    TimerId runAt(Timestamp time,TimerCallback cb);
    TimerId runAfter(double delay,TimerCallback cb);
    TimerId runEvery(double interval,TimerCallback cb);

    void runInLoop(Functor cb);
    bool isInLoopThread()const;
    void queueInLoop(Functor cb);



    public:
    Poller* poller;

    private:
    std::thread::id threadId;
    bool isStop;
    std::unique_ptr<TimerQueue> timerQueue;

    std::mutex mutex;
    bool callingPendingFunctors;
    void doPendingFunctors();
    std::vector<Functor> pendingFunctors;
    void wakeup();
    int wakeupfd;
    std::unique_ptr<Channel> wakeupChannel;
    void handleEventfdRead();



    

};
}
