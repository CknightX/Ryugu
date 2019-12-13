#pragma once
#include <cstdint>


namespace ck
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
        :timer(timer),sequence(seq)
        {
        }

        private:
        Timer* timer;
        // 序号
        int64_t sequence;

    };

}