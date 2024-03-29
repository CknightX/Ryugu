/*
EventLoop
*/
#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include "Ryugu/net/Poller.h"
#include "Ryugu/base/Debug.h"
#include "Ryugu/net/TimerId.h"
#include "Ryugu/base/Timestamp.h"
#include "Ryugu/net/TimerQueue.h"
#include "Ryugu/net/Timer.h"

namespace ryugu
{

	namespace net
	{
		class Channel;


		// 事件处理器
		class EventLoop
		{
		public:
			EventLoop();
			~EventLoop();

			// 启动循环
			void loopOnce(int waitMs);
			void loop();
			void stop() { isStop = true; }

			// 定时器任务 seconds
			TimerId runAt(base::Timestamp time, TimerCallback cb);
			TimerId runAfter(double delay, TimerCallback cb);
			TimerId runEvery(double interval, TimerCallback cb);

			// Channel增删
			void updateChannel(Channel* channel);
			void removeChannel(Channel* channel);
			bool hasChannel(Channel* channel);

			void runInLoop(Functor cb);
			bool isInLoopThread()const;
			void queueInLoop(Functor cb);
			void assertInLoopThread();



		private:
			// 多路复用器
			std::unique_ptr<Poller> poller;


			std::thread::id threadId;
			bool isStop;
			// 定时器队列
			std::unique_ptr<TimerQueue> timerQueue;

			std::mutex mutex;

			bool callingPendingFunctors;
			void doPendingFunctors();
			std::vector<Functor> pendingFunctors;

			void wakeup();
			int wakeupfd;

			std::unique_ptr<Channel> wakeupChannel;
			void handleEventfdRead();

			void abortNotInLoopThread();




		};
	}
}
