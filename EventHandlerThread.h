#pragma once
#include <string>
#include <thread>
#include <condition_variable>
#include <memory>
#include "Utils.h"

namespace ck
{
    class EventHandler;


    class EventHandlerThread : noncopyable
    {
        public:
        EventHandlerThread(const std::string& name);
        ~EventHandlerThread();

        EventHandler* start();

        private:
        void threadFunc();
        EventHandler* handler;
        // 延迟启动
        std::shared_ptr<std::thread> threadPtr;
        std::mutex mutex;
        std::condition_variable condition;

        
    };
}