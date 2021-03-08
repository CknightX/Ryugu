/*
Channel
对底层的fd进行封装，并绑定与之相应的事件处理函数。
放入Poller中进行事件循环
*/
#pragma once
#include <stdint.h>
#include <functional>
#include <memory>
#include "Netheaders.h"

namespace ck 
{
	class EventLoop;
	class Poller;

	using ID = int64_t;
	using EventCallback=std::function<void()>;

	const auto cstReadEvent=EPOLLIN;
	const auto cstWriteEvent=EPOLLOUT;
	const auto cstClientLost=EPOLLRDHUP;


	// 通道，对fd进行封装，用于放入IO复用器中等待事件发生
	class Channel 
	{
		public:
			Channel(EventLoop* _loop, int _fd);
			~Channel(){close();}

		public:
			int getFd() const {return fd;}
			ID getId() const {return id;}	

			uint32_t getEvents() const {return events;}

			// TODO
			void close();


			// 设置读写的回调函数
			void setReadCB(const EventCallback& readcb){readCallback=readcb;}
			void setWriteCB(const EventCallback& writecb){writeCallback=writecb;}

			void setReadCB(EventCallback&& readcb){readCallback=std::move(readcb);}
			void setWriteCB(EventCallback&& writecb){writeCallback=std::move(writecb);}


			// 开启/关闭读写事件监听
			void enableRead(bool enable);
			void enableWrite(bool enable);
			void enableReadWrite(bool readable,bool writable);
            void update();
            void remove();

			bool isReadEnabled(){return events&cstReadEvent;}
			bool isWriteEnabled(){return events&cstWriteEvent;}

			void handleRead(){readCallback();}
			void handleWrite(){writeCallback();}

            void tie(const std::shared_ptr<void>& obj);
        private:
            // for TcpConn
            std::weak_ptr<void> tie_;
            bool tied_;

			EventLoop* loop;
			Poller* poller;
			int fd;
			
			uint32_t events;
			ID id;

			EventCallback readCallback;
            EventCallback writeCallback;
            EventCallback errorCallback;
	};
}
