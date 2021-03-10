#include "TcpConn.h"
#include "Channel.h"
#include "Debug.h"
namespace ryugu
{
    TcpConn::TcpConn(EventLoop* loop, int sockfd, net::InetAddr localAddr, net::InetAddr peerAddr)
        :loop_(loop),
        state_(Invalid),
        channel_(new Channel(loop,sockfd)),
		socket_(new net::Socket(sockfd)),
        localAddr_(localAddr),
        peerAddr_(peerAddr)
    {
        channel_->setReadCB([this]{this->handleRead();});
        channel_->setWriteCB([this]{this->handleWrite();});
    }
    TcpConn::~TcpConn()
    {
        LOG("TcpConnection desconstruction.");
    }
    void TcpConn::setState(State state)
    {
        state_=state;
    }
    int TcpConn::getFd() const
    {
        return channel_->getFd();
    }

    // 暂时先不管TCP状态..
    void TcpConn::handleRead()
    {
        // 将数据读到readBuf中
        fillReadBuf();
        if (readCb)
        {
            readCb(shared_from_this());
        }

    }
    void TcpConn::connectEstablished()
    {
        setState(Connecting);
        channel_->tie(shared_from_this());
        // 线程不安全，所以要放到io线程执行
        channel_->enableRead(true);
    }
    void TcpConn::handleWrite()
    {
        // 写writeBuf里面的内容
        ssize_t sended=_write(writeBuf.getData(),writeBuf.size());
        writeBuf.consume(sended);
        // 写完了
        if (writeBuf.empty())
        {
            // 用户自定义回调
            if (writeCb)
                writeCb(shared_from_this());
            // 关闭可写事件监听
            if (channel_->isWriteEnabled())
                channel_->enableWrite(false);
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
            channel_->enableWrite(true);
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
            ssize_t nbytes=::write(channel_->getFd(),buf+sended,len-sended);
            if (nbytes>0)
            {
                sended+=nbytes;
                continue;
            }
            else if (nbytes<0)
            {
                if (errno==EINTR)
                    continue;
                // 内核写缓冲区满
                else if (errno==EAGAIN)
                {

                    // if (!channel->isWriteEnabled())
                    //     channel->enableWrite(true);
                    break;
                }
                else
                {
                    LOG_ERROR("write error,fd=%d",channel_->getFd());
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
            n = ::read(channel_->getFd(), _buf, 256);
            if (n < 0)
            {
                // 被中断打断
                if (errno == EINTR)
	
                    continue;

                // LT模式读取完毕
                if (errno == EAGAIN)
                    break;
            }
            // 当read返回值为0时说明对端断开连接
            else if (n == 0)
            {
                // 关闭tcp连接，释放channel
                LOG("lost");
                handleClose();
                break;
            }
            else
            {
                readBuf.writeIn(_buf, n);
            }
        }
    }
    void TcpConn::handleClose()
    {
        setState(Disconnected);
        channel_->enableAll(false);
        TcpConnPtr guardThis(shared_from_this());
        //connCb(guardThis);
        closeCb(guardThis);
    }
    void TcpConn::connectDestroyed()
    {
        channel_->remove();
    }


} // namespace ryugu
