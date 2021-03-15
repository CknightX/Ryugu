#pragma once
#include <cstdint>


namespace ryugu
{
	namespace net
	{
		class Timer;

		// 该类暴露给用户
		class TimerId
		{
		public:
			TimerId() : timer(nullptr), sequence(0)
			{
			}

			TimerId(Timer* timer, int64_t seq)
				:timer(timer), sequence(seq)
			{
			}

		private:
			Timer* timer;
			// 序号
			int64_t sequence;

		};
	}

}