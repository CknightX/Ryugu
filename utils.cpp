#include "utils.h"
#include "debug.h"
#include <sys/timerfd.h>

namespace ck
{
    std::string strEvents(uint32_t events)
    {
        //TODO
        return "";
    }
    std::string strErrno(int _errno)
    {
        //TODO
        return "";
    }

    int createTimerfd()
    {
        int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
        if (timerfd<0)
        {
            LOG_ERROR("Failed to create timerfd");
        }
        return timerfd;
    }

}