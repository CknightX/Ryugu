#include "Channel.h"
#include "EventLoop.h"
#include "Debug.h"
#include "Net.h"
#include <atomic>
namespace ryugu
{
	namespace net
	{
		Channel::Channel(EventLoop* _loop, int _fd)
			:loop(_loop), fd(_fd), events(0)
		{
			static std::atomic<int64_t> _id(0);
			id = ++_id;
			// 设置为非阻塞套接字
			if (net::setNonBlocking(_fd) < 0)
			{
				LOG_ERROR("setNonBlocking failed.");
			}

			LOG("new Channel,fd=%d", _fd);
		}
		Channel::~Channel()
		{
			LOG("Channel deleted. fd=%d", fd);
		}

		void Channel::enableRead(bool enable)
		{
			if (enable)
			{
				events |= cstReadEvent;
			}
			else
			{
				events &= ~cstReadEvent;
			}
			update();

		}

		void Channel::enableWrite(bool enable)
		{
			if (enable)
			{
				events |= cstWriteEvent;
			}
			else
			{
				events &= ~cstWriteEvent;
			}
			update();
		}

		void Channel::enableAll(bool enable)
		{
			if (enable)
			{
				events |= cstReadEvent;
				events |= cstWriteEvent;
			}
			else
			{
				events &= ~cstReadEvent;
				events &= ~cstWriteEvent;
			}
			update();
		}

		void Channel::update()
		{
			loop->updateChannel(this);
		}
		void Channel::remove()
		{
			loop->removeChannel(this);
		}
		void Channel::tie(const std::shared_ptr<void>& obj)
		{
			tie_ = obj;
			tied_ = true;
		}
		void Channel::handleRead()
		{
			if (tied_)
			{
				auto guard = tie_.lock();
				if (guard)
				{
					readCallback();
				}
			}
			else
			{
				readCallback();
			}
		}
		void Channel::handleWrite()
		{
			if (tied_)
			{
				auto guard = tie_.lock();
				if (guard)
				{
					writeCallback();
				}
			}
			else
			{
				writeCallback();
			}
		}
	}
}
