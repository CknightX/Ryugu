#include <sys/timerfd.h>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include "TimerQueue.h"
#include <cassert>
#include "Debug.h"

namespace ck
{
    void readTimerfd(int timerfd,Timestamp now)
    {
        uint64_t howmany;
        ssize_t n=::read(timerfd,&howmany,sizeof howmany);
        LOG("TimerQueue::handleRead() %ud at %s",howmany,now.toString().data());
        if (n!=sizeof(howmany))
        {
            LOG_ERROR("readTimerfd error");
        }
    }
    // 从now到when的时间
    timespec timeFromNow(Timestamp when)
    {
        int64_t microseconds=when.getTimestampByMicroSeconds()-Timestamp::getNow().getTimestampByMicroSeconds();
        // 小于100ms按100ms
        microseconds=microseconds<100?100:microseconds;

        timespec ts;
        ts.tv_sec=static_cast<time_t> (microseconds/Timestamp::cstMicroSecondsPerSecond);
        ts.tv_nsec=static_cast<long>(
            (microseconds%Timestamp::cstMicroSecondsPerSecond)*1000
        );
        return ts;
    }


    TimerQueue::TimerQueue(EventLoop* _loop)
    : loop(_loop),
    timerfd(createTimerfd()),
    timerfdChannel(_loop,timerfd),
    timers(),
    callingExpiredTimers(false)
    {
        timerfdChannel.setReadCB(
            std::bind(&TimerQueue::handleRead,this));
        
        timerfdChannel.enableRead(true);
    }

    TimerQueue::~TimerQueue()
    {
        ::close(timerfd);
    }

    TimerId TimerQueue::addTimer(const TimerCallback& cb,Timestamp when,double interval)
    {
        Timer* timer=new Timer(std::move(cb),when,interval);
        addTimerInLoop(timer);
        return TimerId(timer,timer->getSequence());
    }

    void TimerQueue::addTimerInLoop(Timer* timer)
    {
        // 先不进行本线程判断
        
        // 加入
        bool earliestChanged=insert(timer);

        // 是第一个插入的，或者时间间隔是最小的，此时需要重置timerfd
        if (earliestChanged)
        {
            resetTimerfd(timerfd,timer->getExpiration());
        }
    }

    void TimerQueue::resetTimerfd(int timerfd,Timestamp expiration)
    {
        itimerspec newValue,oldValue;
        memset(&newValue,0,sizeof newValue);
        memset(&oldValue,0,sizeof oldValue);
        newValue.it_value=timeFromNow(expiration);
        int ret= ::timerfd_settime(timerfd,0,&newValue,&oldValue);
        if (ret)
        {
            LOG("timerfd_settime()");
        }
    }

    // timerfd触发
    void TimerQueue::handleRead()
    {
        // TODO 本线程判断

        Timestamp now(Timestamp::getNow());
        // 水平触发，需要读出，防止反复触发
        readTimerfd(timerfd,now);

        std::vector<Entry> expired = getExpired(now);

        // 正在执行到期的定时器（多线程）
        callingExpiredTimers=true;

        cancelingTimers.clear();

        // 执行所有到期定时器任务
        for (const Entry& it:expired)
        {
            it.second->run();
        }
        // 执行完毕
        callingExpiredTimers=false;

        // 重置（循环定时器）
        reset(expired,now);
    }
    // 获取now为止到期的timer
    std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
    {
        assert(timers.size()==activeTimers.size());

        std::vector<Entry> expired;
        Entry sentry(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
        // 二分查找第一个>=now的定时器
        auto end=timers.lower_bound(sentry);
        // now最大 || now小于找到的定时器
        assert(end==timers.end()|| now <= end->first);

        // 所有小于now的（到期）定时器
        std::copy(timers.begin(),end,std::back_inserter(expired));

        timers.erase(timers.begin(),end);
        // 删除所有到期定时器
        for (const Entry& it:expired)
        {
            ActiveTimer timer(it.second,it.second->getSequence());
            size_t n=activeTimers.erase(timer);
        }

        assert(timers.size()==activeTimers.size());

        return expired;

    }

    void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
    {
        Timestamp nextExpire;

        for (const Entry& it:expired)
        {
            ActiveTimer timer(it.second,it.second->getSequence());
            // 如果是重复定时器，并且不在已经取消的列表里
            if (it.second->isRepeat()&&cancelingTimers.find(timer)==cancelingTimers.end())
            {
                // 重启定时器
                it.second->restart(now);
                // 插入定时器队列
                insert(it.second);
            }
            else
            {
                delete it.second;
            }
        }

        if (!timers.empty())
        {
            // 新的timerfd触发时间是所有计时器中离现在最近的时间
            nextExpire=timers.begin()->second->getExpiration();
            if (nextExpire.isValid())
            {
                resetTimerfd(timerfd,nextExpire);
            }
        }

    }

    bool TimerQueue::insert(Timer* timer)
    {
        // TODO 判断是不是在当前线程

        // 全部timer都在活跃状态
        assert(timers.size()==activeTimers.size());

        bool earliestChanged=false;
        Timestamp when = timer->getExpiration();
        auto iter=timers.begin();
        // 待插入的timer是否是最早的一个
        if (iter == timers.end() || when < iter->first)
        {
            earliestChanged=true;
        }

        {
        auto result=timers.insert(Entry(when,timer));
        assert(result.second);
        }
        {
        auto result=activeTimers.insert(ActiveTimer(timer,timer->getSequence()));
        assert(result.second);
        }
        return earliestChanged;

    }

    int TimerQueue::createTimerfd()
    {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd < 0)
        {
            LOG_ERROR("Failed to create timerfd");
        }
        return timerfd;
    }

    } // namespace ck
