#include <cstdlib>
#include <cstring>
#include <iostream>
#include "Ryugu/net/Poller.h"
#include "Ryugu/net/Channel.h"
#include "Ryugu/base/log/Logging.h"

namespace ryugu
{

	namespace net
	{

		Epoll::Epoll()
		{
			// 用epoll_create1替换epoll_create
			// EPOLL_CLOEXEC 代表epoll fd 具有执行后关闭的特性
			epollFd = epoll_create1(0);
			if (epollFd < 0)
			{
				LOG_ERROR << "epoll_create error" << errno;
			}
			LOG_DEBUG << "epoll fd=" << epollFd << " created";
		}

		Epoll::~Epoll()
		{
			// 关闭epoll的fd
			::close(epollFd);

			LOG_DEBUG << "poller destroyed fd=" << epollFd;
		}

		void Epoll::addChannel(Channel* ch)
		{

			epoll_event ev;
			memset(&ev, 0, sizeof(ev));
			ev.events = ch->getEvents();
			ev.data.ptr = ch;
			int r = epoll_ctl(epollFd, EPOLL_CTL_ADD, ch->getFd(), &ev);
			if (r < 0)
			{
				LOG_ERROR << "epoll_ctl add failed %d" << errno;
				return;
			}
			liveChannels.insert(ch);
			LOG_DEBUG<<"add ok.";
		}

		void Epoll::updateChannel(Channel* ch)
		{
			epoll_event ev;
			memset(&ev, 0, sizeof ev);
			ev.events = ch->getEvents();
			ev.data.ptr = static_cast<void*>(ch);
			int r = epoll_ctl(epollFd, EPOLL_CTL_MOD, ch->getFd(), &ev);
			if (r < 0)
			{
				LOG_ERROR << "epoll_ctl update failed " << errno;
				return;
			}
		}

		void Epoll::removeChannel(Channel* ch)
		{
			liveChannels.erase(ch);
			int r = epoll_ctl(epollFd, EPOLL_CTL_DEL, ch->getFd(), NULL);
			if (r < 0)
			{
				LOG_ERROR << "epoll_ctl del failed" << errno;
			}
		}
		void Epoll::loopOnce(int waitMs)
		{
			lastActive = epoll_wait(epollFd, activeChannels, cstMaxChannels - 1, waitMs);
			for (int i = 0; i < lastActive; ++i)
			{
				Channel* ch = static_cast<Channel*>(activeChannels[i].data.ptr);

				if (ch)
				{
					auto events = ch->getEvents();
					// 对端断开
					// if (events&cstClientLost)
					// {
					// 	LOG("lost.");
					// 	removeChannel(ch);
					// 	return;
					// }
					if (events & cstWriteEvent)
					{
						LOG_DEBUG << "new write event.";
						ch->handleWrite();
					}
					if (events & cstReadEvent)
					{
						LOG_DEBUG << "new read event.";
						ch->handleRead();
					}
				}
			}
		}

		std::atomic<int64_t> Poller::_id(0);
	} // namespace ryugu
}
