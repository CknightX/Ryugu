#include "Ryugu/net/EventLoopThreadPool.h"
#include "Ryugu/net/EventLoopThread.h"
#include "Ryugu/base/Debug.h"
namespace ryugu
{
	namespace net
	{
		EventLoopThreadPool::EventLoopThreadPool(EventLoop* _baseLoop, const std::string& _name)
			:baseLoop(_baseLoop),
			name(_name),
			started(false),
			numThreads(0),
			next(0)
		{

		}
		EventLoopThreadPool::~EventLoopThreadPool()
		{

		}

		void EventLoopThreadPool::start()
		{
			if (started)
				return;

			started = true;

			for (int i = 0; i < numThreads; ++i)
			{
				LOG("The number of threads:%d", numThreads);
				std::string tname = name + std::to_string(i);
				EventLoopThread* t = new EventLoopThread(tname);
				threads.push_back(std::unique_ptr<EventLoopThread>(t));
				handlers.push_back(t->start());
			}
		}

		EventLoop* EventLoopThreadPool::getOneLoop()
		{
			EventLoop* handler = baseLoop;

			if (!handlers.empty())
			{
				handler = handlers[next++];
				if (static_cast<size_t>(next) >= handlers.size())
					next = 0;
			}
			return handler;
		}

		std::vector<EventLoop*> EventLoopThreadPool::getAllLoop()
		{

			if (handlers.empty())
			{
				return std::vector<EventLoop*>{baseLoop};
			}
			else
			{
				return handlers;
			}
		}
	}
}
