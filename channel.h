#pragma once
#include <stdint.h>
#include <functional>
#include "netheaders.h"




namespace ck 
{
	class EventHandler;
	class Poller;

	using ID = int64_t;
	using Task=std::function<void()>;

	const auto cstReadEvent=EPOLLIN;
	const auto cstWriteEvent=EPOLLOUT;
	const auto cstClientLost=EPOLLRDHUP;


	// 通道，对fd进行封装，用于放入IO复用器中等待事件发生
	class Channel 
	{
		public:
			Channel(EventHandler* _eh, int _fd, uint32_t _events);
			~Channel(){close();}

		public:
			int getFd() const {return fd;}

			ID getId() const {return id;}	
			uint32_t getEvents() const {return events;}

			// TODO
			void close();


			// 设置读写的回调函数，均为void()
			void setReadCB(const Task& readcb){readCallback=readcb;}
			void setWriteCB(const Task& writecb){writeCallback=writecb;}

			void setReadCB(Task&& readcb){readCallback=std::move(readcb);}
			void setWriteCB(Task&& writecb){writeCallback=std::move(writecb);}


			void enableRead(bool enable);
			void enableWrite(bool enable);
			void enableReadWrite(bool readable,bool writable);

			bool isReadEnabled(){return events&cstReadEvent;}
			bool isWriteEnabled(){return events&cstWriteEvent;}

			void handleRead(){readCallback();}
			void handleWrite(){writeCallback();}

		protected:
			EventHandler* eh;
			Poller* poller;
			int fd;
			
			uint32_t events;
			ID id;
			// channel的回调函数不需要参数，需要的参数已经被绑定到lambda闭包中了
			Task readCallback, writeCallback, errorCallback;
		private:

	};
}
