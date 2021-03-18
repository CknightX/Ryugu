#include <assert.h>
#include "TcpConn.h"
#include "Channel.h"
#include "Debug.h"
#include "EventLoop.h"
namespace ryugu
{
	namespace net
	{
		TcpConn::TcpConn(EventLoop* loop, int sockfd, net::InetAddr localAddr, net::InetAddr peerAddr)
			:loop_(loop),
			state_(Connecting),
			channel_(new Channel(loop, sockfd)),
			socket_(new net::Socket(sockfd)),
			localAddr_(localAddr),
			peerAddr_(peerAddr)
		{
			channel_->setReadCB([this] {this->handleRead(); });
			channel_->setWriteCB([this] {this->handleWrite(); });
		}
		TcpConn::~TcpConn()
		{
			LOG("TcpConnection desconstruction.");
		}
		void TcpConn::setState(State state)
		{
			state_ = state;
		}
		int TcpConn::getFd() const
		{
			return channel_->getFd();
		}

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
			assert(state_ == Connecting);
			channel_->tie(shared_from_this());
			// 线程不安全，所以本函数要放到io线程执行
			channel_->enableRead(true);

			setState(Connected);
		}
		void TcpConn::handleWrite()
		{
			// 写writeBuf里面的内容
			ssize_t sended = _write(writeBuf.getData(), writeBuf.size());
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
		void TcpConn::send(const char* buf, size_t len)
		{
			// 先尽可能发送
			auto sended = _write(buf, len);

			// 没发完,而且此时写缓冲区已满,将没发完的数据存入buffer中
			if (sended < len)
			{
				// 剩下内容放到writeBuf中
				writeBuf.writeIn(buf + sended, len - sended);
				// 由epoll进行剩下的工作
				channel_->enableWrite(true);
			}
		}
		void TcpConn::send(const std::string& str)
		{
			send(str.c_str(), str.size());
		}
		std::string TcpConn::getInput()
		{
			return readBuf.readOutAsString(readBuf.size());
		}

		size_t TcpConn::_write(const char* buf, size_t len)
		{
			size_t sended = 0;
			while (len > sended)
			{
				ssize_t nbytes = ::write(channel_->getFd(), buf + sended, len - sended);
				if (nbytes > 0)
				{
					sended += nbytes;
					continue;
				}
				else if (nbytes < 0)
				{
					if (errno == EINTR)
						continue;
					// 内核写缓冲区满
					else if (errno == EAGAIN)
					{

						// if (!channel->isWriteEnabled())
						//     channel->enableWrite(true);
						break;
					}
					else
					{
						LOG_ERROR("write error,fd=%d", channel_->getFd());
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
			loop_->assertInLoopThread();
			assert(state_ == Connected || state_ == DisConnecting);
			channel_->enableAll(false);
			TcpConnPtr guardThis(shared_from_this());
			//connCb(guardThis);
			// call TcpServer::removeConnection
			closeCb(guardThis);
			setState(DisConnected);
		}
		// called by TcpServer::removeConnection
		void TcpConn::connectDestroyed()
		{
			loop_->assertInLoopThread();
			// 这里与上面重复，因为有时可以不经由handleClose()直接调用connectDestroyed()
			if (state_ == Connected)
			{
				setState(DisConnected);
				channel_->enableAll(false);
			}
			channel_->remove();
		}
		void TcpConn::shutdown()
		{
			if (state_ == Connected)
			{
				setState(DisConnecting);
				loop_->runInLoop([this] {
					shutdownInLoop();
				});
			}
		}
		void TcpConn::forceClose()
		{
			// 连接中或者由shutdown引起的半关闭状态
			if (state_ == Connected || state_ == DisConnecting)
			{
				setState(DisConnecting);
				auto guardThis = shared_from_this();
				loop_->queueInLoop([guardThis] {
					guardThis->forceCloseInLoop();
				});
			}
		}
		void TcpConn::forceCloseInLoop()
		{
			loop_->assertInLoopThread();
			if (state_ == Connected || state_ == DisConnecting)
			{
				// 如同read返回0
				handleClose();
			}
		}
		void TcpConn::shutdownInLoop()
		{
			loop_->assertInLoopThread();
			// TODO : judge channel is writing ?
			// 只关闭写端，要求对端read==0时会断开
			socket_->shutdownWrite();
		}


	} // namespace ryugu
}
