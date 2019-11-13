#include "conn.h"
#include "channel.h"
#include "debug.h"
namespace ck
{
    TcpConn::TcpConn()
        :channel(nullptr),handler(nullptr),readcb(nullptr),writecb(nullptr),state(State::Invalid)
    {

    }
    // 作为服务端，创建TcpConn发生在accept之后，握手阶段已经结束
    void TcpConn::attach(EventHandler* _handler, int fd, Ipv4Addr _local, Ipv4Addr _peer)
    {
        LOG("attch fd=%d",fd);

        handler=_handler;
        local=_local;
        peer=_peer;

        if (channel)
        {
            delete channel;
        }
        else
        {
            // 创建客户channel
            channel=new Channel(handler,fd,cstReadEvent);

            TcpConnPtr con=shared_from_this();
            channel->setReadCB([=]{con->handleRead(con);});
            channel->setWriteCB([=]{con->handleWrite(con);});

        }
    }

    // 暂时先不管TCP状态..
    void TcpConn::handleRead(const TcpConnPtr& conn)
    {
        // 将数据读到readBuf中
        fillReadBuf();
        if (readcb)
        {
            readcb(conn);
        }

    }
    void TcpConn::handleWrite(const TcpConnPtr& conn)
    {
        // 写writeBuf里面的内容
        ssize_t sended=_write(writeBuf.getData(),writeBuf.size());
        writeBuf.consume(sended);
        // 写完了
        if (writeBuf.empty())
        {
            // 用户自定义回调
            if (writecb)
                writecb(conn);
            // 关闭可写事件监听
            if (channel->isWriteEnabled())
                channel->enableWrite(false);
        }

    }
    void TcpConn::send(const char* buf,size_t len)
    {
        // 先尽可能发送
        auto sended=_write(buf,len);

        // 没发完,而且此时写缓冲区已满,将没发完的数据存入buffer中
        if (sended<len)
        {
            // 剩下内容放到writeBuf中
            writeBuf.writeIn(buf+sended,len-sended);
            // 由epoll进行剩下的工作
            channel->enableWrite(true);
        }
    }
    void TcpConn::send(const std::string& str)
    {
        send(str.c_str(),str.size());
    }
    std::string TcpConn::getInput()
    {
        return readBuf.readOutAsString(readBuf.size());
    }

    size_t TcpConn::_write(const char* buf,size_t len)
    {
        size_t sended=0;
        while(len>sended)
        {
            ssize_t nbytes=::write(channel->getFd(),buf+sended,len-sended);
            if (nbytes>0)
            {
                sended+=nbytes;
                continue;
            }
            else if (nbytes<0)
            {
                if (errno==EINTR)
                    continue;
                // 写缓冲区满
                else if (errno==EAGAIN)
                {

                    // if (!channel->isWriteEnabled())
                    //     channel->enableWrite(true);
                    break;
                }
                else
                {
                    LOG_ERROR("write error,fd=%d",channel->getFd());
                }
                
            }
        }
        return sended;
    }
    void TcpConn::fillReadBuf()
    {
        static char _buf[256];
        int n;
        while (1)
        {
            n = ::read(channel->getFd(), _buf, 256);
            if (n < 0)
            {
                // 被中断打断
                if (errno == EINTR)
                    continue;

                // LT模式读取完毕
                if (errno == EAGAIN)
                    break;
            }
            // 对端断开连接
            else if (n == 0)
            {
                close();
                break;
            }
            else
            {
                readBuf.writeIn(_buf, n);
            }
        }
    }
    void TcpConn::close()
    {
        readcb=writecb=nullptr;
        delete channel;
        channel=nullptr;
    }
} // namespace ck