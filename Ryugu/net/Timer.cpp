#include "Ryugu/net/Timer.h"
#include "Ryugu/base/Timestamp.h"
namespace ryugu
{
	namespace net
	{
		void Timer::restart(base::Timestamp now)
		{
			if (repeat)
			{
				// 循环定时器将下次到期时间设置为now+间隔
				now.addTime(interval);
				expiration = now;
			}
			else
			{
				// 不是循环定时器就将当前定时器时间设为非法值
				expiration = base::Timestamp::invalid();
			}
		}
		std::atomic<int64_t> Timer::seqCreated(0);
	}
}