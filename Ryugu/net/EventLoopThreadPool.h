#pragma once
#include <string>
#include <vector>
#include "Ryugu/base/Utils.h"
#include "Ryugu/base/Noncopyable.h"
#include "Ryugu/net/Callbacks.h"

namespace ryugu
{
	namespace net
	{
		class EventLoop;
		class EventLoopThread;

		// one loop per thread
		class EventLoopThreadPool : base::Noncopyable
		{
		public:
			EventLoopThreadPool(EventLoop* _baseLoop, const std::string& _name);
			~EventLoopThreadPool();
			void setThreadNum(int nums) { numThreads = nums; }
			void start();

			EventLoop* getOneLoop();
			std::vector<EventLoop*> getAllLoop();

			bool isStarted() const { return started; }
			const std::string& getName() const { return name; }

		private:
			EventLoop* baseLoop;
			std::string name;
			bool started;
			int numThreads;
			// 下一次选择的handler序号
			int next;
			std::vector<std::unique_ptr<EventLoopThread>> threads;
			std::vector<EventLoop*> handlers;
		};
	}
}
