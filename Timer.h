#pragma once
#include "utils.h"
#include "Timestamp.h"
#include <functional>

namespace ck
{
    using TimerCallback=std::function<void()>;

    class Timer : noncopyable
    {
        public:
        Timer(TimerCallback cb,Timestamp when,double _interval)
        :callback(std::move(cb)), expiration(when),interval(_interval),
        repeat(_interval> 0.0), sequence(0)
        {

        }

        void run() const
        {
            callback();
        }

        Timestamp getExpiration() const {return expiration;}
        int64_t getSequence() const {return sequence;}
        bool isRepeat() const {return repeat;}

        void restart(Timestamp now);




        private:
        const TimerCallback callback;
        // 终止时间
        Timestamp expiration;

        // 重复性定时器的时间间隔
        const double interval;
        const bool repeat;
        const int64_t sequence;

    };
}

