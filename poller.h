/*
Poller
抽象系统底层的IO复用机制
*/




#pragma once

#include <set>
#include <atomic>
#include "netheaders.h"



namespace ck 
{
	class Epoll;


	class Channel;
	const int cstMaxChannels=20;

	// IO复用器基类
	class Poller 
	{
		public:
			int64_t id;
			int lastActive;
			Poller()
				:lastActive(-1)
			{
				static std::atomic<int64_t> _id(0);
				id=++_id;
			}

			virtual void addChannel(Channel* event)=0;
			virtual void removeChannel(Channel* event)=0;
			virtual void updateChannel(Channel* event)=0;

			virtual void loopOnce(int waitMs)=0;


			virtual ~Poller(){}

	};


	// Linux下的Epoll模型
	class Epoll final: public Poller  
	{
		public:
			Epoll();
			~Epoll();

			int epollFd;
			std::set<Channel*> liveChannels;
			epoll_event activeChannels[cstMaxChannels];

			void addChannel(Channel* ch) override;
			void removeChannel(Channel* ch) override;
			void updateChannel(Channel* ch) override;

			void loopOnce(int waitMs) override;

	};


	class PollerFactory
	{
		public:
		static Poller* getPoller(){return new Epoll();}
	};
}
