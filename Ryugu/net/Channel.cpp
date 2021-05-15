#include "Ryugu/net/Channel.h"
#include "Ryugu/net/EventLoop.h"
#include "Ryugu/net/SocketsOps.h"
#include "Ryugu/base/log/Logging.h"
#include <atomic>
namespace ryugu
{
	namespace net
	{
		Channel::Channel(EventLoop* _loop, int _fd)
			:loop(_loop), fd(_fd), events(0),tied_(false),inLoop_(false)
		{
			static std::atomic<int64_t> _id(0);
			id = ++_id;
			// 设置为非阻塞套接字
			if (sockets::setNonBlocking(_fd) < 0)
			{
				LOG_ERROR << "setNonBlocking failed";
			}
			LOG_DEBUG << "Channel construct, fd=" << fd;
		}
		Channel::~Channel()
		{
			LOG_DEBUG << "Channel destruct, fd=" << fd;
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
