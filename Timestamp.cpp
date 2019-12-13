#include "Timestamp.h"

namespace ck
{
    std::string Timestamp::toString() const
    {
        char buf[32] = {0};
        int64_t seconds = microSecondsSinceLastPoll / cstMicroSecondsPerSecond;
        int64_t microseconds = microSecondsSinceLastPoll % cstMicroSecondsPerSecond;
        snprintf(buf, sizeof(buf) - 1, "%" "lld" ".%06" "lld" "", seconds, microseconds);
        return buf;
    }
}