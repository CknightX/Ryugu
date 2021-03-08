#include "Channel.h"
#include "EventLoop.h"
#include "Debug.h"
#include "Net.h"
#include <atomic>
namespace ck
{
    Channel::Channel(EventLoop* _loop,int _fd, uint32_t _events)
        :loop(_loop),fd(_fd),events(_events)
    {
        // 设置为非阻塞套接字
        if (net::setNonBlocking(_fd)<0)
        {
            LOG_ERROR("setNonBlocking failed.");
        }

        // ID++
        static std::atomic<int64_t> _id(0);
        id=++_id;

        // poller->addChannel(this);
        loop->runInLoop([this]{this->loop->updateChannel(this);});

        LOG("new Channel,fd=%d",_fd);
    }

    void Channel::enableRead(bool enable)
    {
        if (enable)
        {
            events|=cstReadEvent;
        }
        else
        {
            events&=~cstReadEvent;
        }

    }

    void Channel::enableWrite(bool enable)
    {
        if (enable)
        {
            events|=cstWriteEvent;
        }
        else
        {
            events&=~cstWriteEvent;
        }

    }

    void Channel::enableReadWrite(bool readable,bool writeable)
    {
        enableRead(readable);
        enableWrite(writeable);
    }

    void Channel::close()
    {
        LOG("close channel fd %d",fd);
        loop->removeChannel(this);
        ::close(fd);
        fd=-2;
        // ?
        //handleRead();
    }
    void Channel::update()
    {
        loop->updateChannel(this);
    }
    void Channel::remove()
    {
        loop->removeChannel(this);
    }

}
