#pragma once
#include <string>
#include <thread>
#include <condition_variable>
#include <memory>
#include "Ryugu/base/Utils.h"
#include "Ryugu/base/Noncopyable.h"

namespace ryugu
{
	namespace net
	{
		class EventLoop;

		class EventLoopThread : base::Noncopyable
		{
		public:
			EventLoopThread(const std::string& name);
			~EventLoopThread();

			EventLoop* start();

		private:
			void threadFunc();
			EventLoop* loop;
			// 延迟启动
			std::unique_ptr<std::thread> threadPtr;
			std::mutex mutex;
			std::condition_variable condition;
			bool exiting;
		};
	}
}
