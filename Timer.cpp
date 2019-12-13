#include "Timer.h"
namespace ck
{
    void Timer::restart(Timestamp now)
    {
        if (repeat)
        {
            now.addTime(interval);
            expiration=now;
        }
        else
        {
            expiration=Timestamp::invalid();
        }
    }
}