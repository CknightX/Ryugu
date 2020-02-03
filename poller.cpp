#include "poller.h"
#include "debug.h"
#include "channel.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace ck 
{

Epoll *globalEpoll;
Epoll::Epoll()
{
	globalEpoll = this;
	// 用epoll_create1替换epoll_create
	// EPOLL_CLOEXEC 代表epoll fd 具有执行后关闭的特性
	epollFd = epoll_create1(0);
	if (epollFd < 0)
	{
		LOG_ERROR("epoll_create error %d", errno);
	}
	LOG("poller epoll %d created", epollFd);
}

Epoll::~Epoll()
{
	LOG("destroying poller %d", epollFd);
	while (liveChannels.size())
		(*liveChannels.begin())->close();

	// 关闭epoll的fd
	::close(epollFd);

	LOG("poller %d destroyed", epollFd);
}

void Epoll::addChannel(Channel *ch)
{

	epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = ch->getEvents();
	ev.data.ptr = ch;
	LOG("adding channel %lld events %u fd %d epoll %d", (long long)ch->getId(), ch->getEvents(), ch->getFd(), epollFd);
	int r = epoll_ctl(epollFd, EPOLL_CTL_ADD, ch->getFd(), &ev);
	if (r < 0)
	{
		LOG("epoll_ctl add failed %d", errno);
		return;
	}
	liveChannels.insert(ch);
	LOG("add ok.");
}

void Epoll::updateChannel(Channel *ch)
{
	epoll_event ev;
	memset(&ev, 0, sizeof ev);
	ev.events = ch->getEvents();
	ev.data.ptr = static_cast<void *>(ch);
	LOG("modifying channel %lld fd %d events %u epoll %d", (long long)ch->getId(), ch->getEvents(), ch->getFd(), epollFd);
	int r = epoll_ctl(epollFd, EPOLL_CTL_ADD, ch->getFd(), &ev);
	if (r < 0)
	{
		LOG_ERROR("epoll_ctl add failed %d", errno);
		return;
	}
}

void Epoll::removeChannel(Channel *ch)
{
	liveChannels.erase(ch);
	int r = epoll_ctl(epollFd, EPOLL_CTL_DEL, ch->getFd(), NULL);
	if (r < 0)
	{
		LOG_ERROR("epoll_ctl del failed %d", errno);
	}
}
void Epoll::loopOnce(int waitMs)
{
	lastActive = epoll_wait(epollFd, activeChannels, cstMaxChannels - 1, waitMs);
	for (int i = 0; i < lastActive; ++i)
	{
		Channel *ch = static_cast<Channel *>(activeChannels[i].data.ptr);

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
				LOG("new write event.");
				ch->handleWrite();
			}
			if (events & cstReadEvent)
			{
				LOG("new read event.");
				ch->handleRead();
			}
		}
	}
}

std::atomic<int64_t> Poller::_id(0);
} // namespace ck
