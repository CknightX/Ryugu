#include "EventHandler.h"
#include <unistd.h>
#include <sys/eventfd.h>
namespace ck{


int createEventfd()
{
    int evfd=::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if (evfd<0)
    {
        LOG_ERROR("create eventfd failed.");
    }

    return evfd;
}


EventHandler::EventHandler()
:isStop(false),
poller(PollerFactory::getPoller()),
timerQueue(new TimerQueue(this)),
wakeupfd(createEventfd()),
wakeupChannel(new Channel(this,wakeupfd,cstReadEvent)),
threadId(std::this_thread::get_id())
{
    wakeupChannel->setReadCB(std::bind(&EventHandler::handleEventfdRead,this));
}
EventHandler::~EventHandler()
{
    
}

void EventHandler::loopOnce(int waitMs)
{
    poller->loopOnce(waitMs);
}
void EventHandler::loop()
{
    while(!isStop)
    {
        loopOnce(500);
        doPendingFunctors();
    }
}

TimerId EventHandler::runAt(Timestamp time,TimerCallback cb)
{
    return timerQueue->addTimer(std::move(cb),time,0.0);
}
TimerId EventHandler::runAfter(double delay, TimerCallback cb)
{
    auto time = Timestamp::getNow().addTime(delay);
    return runAt(time,std::move(cb));
}
TimerId EventHandler::runEvery(double interval, TimerCallback cb)
{
    auto time=Timestamp::getNow().addTime(interval);
    return timerQueue->addTimer(std::move(cb),time,interval);
}

void EventHandler::runInLoop(Functor cb)
{
    // 如果在这个handler loop的线程执行cb，则直接执行，否则添加到对应的任务队列
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
        wakeup();
    }
}
bool EventHandler::isInLoopThread() const
{
    return threadId==std::this_thread::get_id();
}

void EventHandler::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        pendingFunctors.push_back(std::move(cb));
        MARK
    }
    if (!isInLoopThread() || callingPendingFunctors)
    {
        wakeup();
        MARK
    }
}

void EventHandler::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors=true;
    {
        std::unique_lock<std::mutex> lock(mutex);
        functors.swap(pendingFunctors);
    }

    for (const Functor& functor:functors)
    {
        functor();
    }
    callingPendingFunctors=false;
}

void EventHandler::wakeup()
{
    uint64_t one=1;
    ssize_t n=::write(wakeupfd,&one,sizeof one);
    if (n!=sizeof one)
    {
        LOG_ERROR("wakeup error");
    }
}
void EventHandler::handleEventfdRead()
{
    uint64_t one=1;
    ssize_t n=::read(wakeupfd,&one,sizeof one);
    if (n!=sizeof one)
    {
        LOG_ERROR("read eventfd failed.");
    }
    MARK

}
}