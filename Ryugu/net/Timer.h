#pragma once
#include "Ryugu/base/Utils.h"
#include "Ryugu/base/Timestamp.h"
#include "Ryugu/net/Callbacks.h"
#include "Ryugu/base/Noncopyable.h"
#include <functional>
#include <atomic>

namespace ryugu
{

	namespace net
	{
		class Timer : base::Noncopyable
		{
		public:
			Timer(TimerCallback cb, base::Timestamp when, double _interval)
				:callback(std::move(cb)), expiration(when), interval(_interval),
				repeat(_interval > 0.0), sequence(seqCreated++)
			{

			}

			void run() const
			{
				callback();
			}
			base::Timestamp getExpiration() const { return expiration; }
			int64_t getSequence() const { return sequence; }
			bool isRepeat() const { return repeat; }

			void restart(base::Timestamp now);




		private:
			const TimerCallback callback;
			// 终止时间
			base::Timestamp expiration;

			static std::atomic<int64_t> seqCreated;

			// 重复性定时器的时间间隔
			const double interval;
			const bool repeat;
			const int64_t sequence;

		};
	}
}

