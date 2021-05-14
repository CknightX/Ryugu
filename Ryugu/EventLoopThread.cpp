#include "EventLoopThread.h"
#include "EventLoop.h"

namespace ryugu
{
	namespace net
	{
		EventLoopThread::EventLoopThread(const std::string& name)
			:exiting(false), threadPtr(nullptr),loop(nullptr)
		{

		}
		EventLoop* EventLoopThread::start()
		{
			threadPtr = std::make_unique<std::thread>(std::bind(&EventLoopThread::threadFunc, this));
			EventLoop* _loop = nullptr;

			{
				std::unique_lock<std::mutex> mux(mutex);
				while (loop == nullptr)
				{
					condition.wait(mux);
				}
				_loop = loop;

			}
			LOG("create one thread.");
			return _loop;
		}
		void EventLoopThread::threadFunc()
		{
			EventLoop localLoop;
			// 局部作用域，为了尽快释放lock_guard
			{
				std::lock_guard<std::mutex> mux(mutex);
				loop = &localLoop;
				condition.notify_one();
			}

			localLoop.loop();

			std::lock_guard<std::mutex> mux(mutex);
			loop = nullptr;

		}

		EventLoopThread::~EventLoopThread()
		{
			exiting = true;
			if (loop)
			{
				loop->stop();
				threadPtr->join();
			}
		}
	}
}
