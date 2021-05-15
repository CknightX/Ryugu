#include <sys/time.h>
#include <inttypes.h>
#include <cstdio>
#include "Ryugu/base/Timestamp.h"

namespace ryugu
{
	namespace base
	{
		std::string Timestamp::toString() const
		{
			char buf[32] = { 0 };
			int64_t seconds = microSecondsSinceLastPoll_ / cstMicroSecondsPerSecond;
			int64_t microseconds = microSecondsSinceLastPoll_ % cstMicroSecondsPerSecond;
			snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
			return buf;
		}
		std::string Timestamp::toPrettyString(bool showMicroseconds) const
		{
			char buf[64] = { 0 };
			time_t seconds = static_cast<time_t>(microSecondsSinceLastPoll_ / cstMicroSecondsPerSecond);
			struct tm tm_time;
			gmtime_r(&seconds, &tm_time);

			if (showMicroseconds)
			{
				int microseconds = static_cast<int>(microSecondsSinceLastPoll_ % cstMicroSecondsPerSecond);
				snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
					tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
					tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
					microseconds);
			}
			else
			{
				snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
					tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
					tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
			}
			return buf;


		}
		Timestamp Timestamp::getNow()
		{
			timeval tv;
			gettimeofday(&tv, NULL);
			int64_t seconds = tv.tv_sec;
			return Timestamp(seconds * cstMicroSecondsPerSecond + tv.tv_usec);
		}
	}
}