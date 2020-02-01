#include "EventHandler.h"
namespace ck{

EventHandler::EventHandler()
:isStop(false),
poller(PollerFactory::getPoller()),
timerQueue(new TimerQueue(this))
{

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

}