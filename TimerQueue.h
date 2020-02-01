#pragma once

#include <set>
#include <utility>
#include <vector>
#include "utils.h"
#include "TimerId.h"
#include "Timer.h"
#include "channel.h"


namespace ck
{
class EventHandler;

class TimerQueue : noncopyable
{
    public:
    TimerQueue(EventHandler* _handler);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,Timestamp when,double interval);

    void cancel(TimerId timerId);

    private:
    using Entry=std::pair<Timestamp,Timer*>;
    using TimerList=std::set<Entry>;
    using ActiveTimer=std::pair<Timer*,int64_t>;
    using ActiveTimerSet=std::set<ActiveTimer>;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    // timerfd可读时的回调函数, 检测所有到期Timer
    void handleRead();
    
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired,Timestamp now);

    bool insert(Timer* timer);

    void resetTimerfd(int timerfd,Timestamp expiration);


    static int createTimerfd();

    EventHandler* handler;
    const int timerfd;
    Channel timerfdChannel;

    // 所有定时器=活动+cancel
    TimerList timers;

    // 活动的定时器
    ActiveTimerSet activeTimers;
    bool callingExpiredTimers;
    ActiveTimerSet cancelingTimers;

};


}