#pragma once
#include "Utils.h"
#include "Timestamp.h"
#include "Callbacks.h"
#include "Noncopyable.h"
#include <functional>
#include <atomic>

namespace ryugu
{

	namespace net
	{
		class Timer : noncopyable
		{
		public:
			Timer(TimerCallback cb, Timestamp when, double _interval)
				:callback(std::move(cb)), expiration(when), interval(_interval),
				repeat(_interval > 0.0), sequence(seqCreated++)
			{

			}

			void run() const
			{
				callback();
			}
			Timestamp getExpiration() const { return expiration; }
			int64_t getSequence() const { return sequence; }
			bool isRepeat() const { return repeat; }

			void restart(Timestamp now);




		private:
			const TimerCallback callback;
			// 终止时间
			Timestamp expiration;

			static std::atomic<int64_t> seqCreated;

			// 重复性定时器的时间间隔
			const double interval;
			const bool repeat;
			const int64_t sequence;

		};
	}
}

