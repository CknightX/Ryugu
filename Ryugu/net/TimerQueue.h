#pragma once

#include <set>
#include <utility>
#include <vector>
#include "Ryugu/base/Utils.h"
#include "Ryugu/net/TimerId.h"
#include "Ryugu/net/Timer.h"
#include "Ryugu/net/Channel.h"


namespace ryugu
{
	namespace net
	{
		class EventLoop;

		class TimerQueue : base::Noncopyable
		{
		public:
			TimerQueue(EventLoop* _loop);
			~TimerQueue();

			TimerId addTimer(const TimerCallback& cb, base::Timestamp when, double interval);

			void cancel(TimerId timerId);

		private:
			using Entry = std::pair<base::Timestamp, Timer*>;
			using TimerList = std::set<Entry>;
			using ActiveTimer = std::pair<Timer*, int64_t>;
			using ActiveTimerSet = std::set<ActiveTimer>;

			void addTimerInLoop(Timer* timer);
			void cancelInLoop(TimerId timerId);
			// timerfd可读时的回调函数, 检测所有到期Timer
			void handleRead();

			std::vector<Entry> getExpired(base::Timestamp now);
			void reset(const std::vector<Entry>& expired, base::Timestamp now);

			bool insert(Timer* timer);

			void resetTimerfd(int timerfd, base::Timestamp expiration);


			static int createTimerfd();

			EventLoop* loop;
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
}
