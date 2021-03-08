#pragma once
#include <string>
#include <thread>
#include <condition_variable>
#include <memory>
#include "Utils.h"

namespace ck
{
    class EventLoop;


    class EventLoopThread : noncopyable
    {
        public:
        EventLoopThread(const std::string& name);
        ~EventLoopThread();

        EventLoop* start();

        private:
        void threadFunc();
        EventLoop* handler;
        // 延迟启动
        std::shared_ptr<std::thread> threadPtr;
        std::mutex mutex;
        std::condition_variable condition;

        
    };
}
