#include "EventLoop.h"
#include <unistd.h>
#include <sys/eventfd.h>
namespace ryugu
{
	namespace net
	{
		int createEventfd()
		{
			int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
			if (evfd < 0)
			{
				LOG_ERROR("create eventfd failed.");
			}

			return evfd;
		}

		EventLoop::EventLoop()
			:isStop(false),
			poller(PollerFactory::getPoller()),
			timerQueue(new TimerQueue(this)),
			wakeupfd(createEventfd()),
			wakeupChannel(new Channel(this, wakeupfd)),
			threadId(std::this_thread::get_id())
		{
			wakeupChannel->enableRead(true);
			wakeupChannel->setReadCB(std::bind(&EventLoop::handleEventfdRead, this));
		}
		EventLoop::~EventLoop()
		{
			LOG("EventLoop destruct");
			wakeupChannel->enableAll(false);
			wakeupChannel->remove();
			::close(wakeupfd);
		}

		void EventLoop::loopOnce(int waitMs)
		{
			poller->loopOnce(waitMs);
		}
		void EventLoop::loop()
		{
			while (!isStop)
			{
				loopOnce(500);
				doPendingFunctors();
			}
		}

		TimerId EventLoop::runAt(Timestamp time, TimerCallback cb)
		{
			return timerQueue->addTimer(std::move(cb), time, 0.0);
		}
		TimerId EventLoop::runAfter(double delay, TimerCallback cb)
		{
			auto time = Timestamp::getNow().addTime(delay);
			return runAt(time, std::move(cb));
		}
		TimerId EventLoop::runEvery(double interval, TimerCallback cb)
		{
			auto time = Timestamp::getNow().addTime(interval);
			return timerQueue->addTimer(std::move(cb), time, interval);
		}

		void EventLoop::runInLoop(Functor cb)
		{
			// 如果在这个handler loop的线程执行cb，则直接执行，否则添加到对应的任务队列
			if (isInLoopThread())
			{
				cb();
			}
			else
			{
				queueInLoop(cb);
				wakeup();
			}
		}
		bool EventLoop::isInLoopThread() const
		{
			return threadId == std::this_thread::get_id();
		}
		void EventLoop::assertInLoopThread()
		{
			if (!isInLoopThread())
			{
				abortNotInLoopThread();
			}
		}
		void EventLoop::abortNotInLoopThread()
		{
			LOG_ERROR("EventLoop::abortNotInLoopThread");
		}

		void EventLoop::queueInLoop(Functor cb)
		{
			{
				std::unique_lock<std::mutex> lock(mutex);
				pendingFunctors.push_back(std::move(cb));
			}
			if (!isInLoopThread() || callingPendingFunctors)
			{
				wakeup();
			}
		}

		void EventLoop::doPendingFunctors()
		{
			std::vector<Functor> functors;
			callingPendingFunctors = true;
			{
				std::unique_lock<std::mutex> lock(mutex);
				functors.swap(pendingFunctors);
			}

			for (const Functor& functor : functors)
			{
				functor();
			}
			callingPendingFunctors = false;
		}

		void EventLoop::wakeup()
		{
			uint64_t one = 1;
			ssize_t n = ::write(wakeupfd, &one, sizeof one);
			if (n != sizeof one)
			{
				LOG_ERROR("wakeup error");
			}
		}
		void EventLoop::handleEventfdRead()
		{
			uint64_t one = 1;
			ssize_t n = ::read(wakeupfd, &one, sizeof one);
			if (n != sizeof one)
			{
				LOG_ERROR("read eventfd failed.");
			}

		}
		void EventLoop::updateChannel(Channel* channel)
		{
			poller->updateChannel(channel);
		}
		void EventLoop::removeChannel(Channel* channel)
		{
			poller->removeChannel(channel);
		}
		bool EventLoop::hasChannel(Channel* channel)
		{
			// TODO
			return true;
		}
	}
}
