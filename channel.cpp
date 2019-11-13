#include "channel.h"
#include "eventhandler.h"
#include "debug.h"
namespace ck
{
Channel::Channel(EventHandler* _eh,int _fd, uint32_t _events)
    :eh(_eh),fd(_fd),events(_events),poller(_eh->poller)
{
    LOG("new Channel,fd=%d",_fd);
    poller->addChannel(this);
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
    poller->removeChannel(this);
    ::close(fd);
    fd=-1;
    // ?
    //handleRead();
}

}