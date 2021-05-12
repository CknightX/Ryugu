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
			peerAddr_(peerAddr),
			context_(nullptr)
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
			loop_->assertInLoopThread();
			// 将数据读到readBuf中
			ssize_t n = readBuf.writeInFromFd(channel_->getFd());
			if (n > 0 && readCb)
			{
				readCb(shared_from_this());
			}
			else if (n == 0)
			{
				handleClose();
			}
			else
			{
				LOG_ERROR("TcpConn::handleRead");
			}
		}
		void TcpConn::connectEstablished()
		{
			assert(state_ == Connecting);
			channel_->tie(shared_from_this());
			// 该语句导致本函数线程不安全，所以本函数要放到io线程执行
			channel_->enableRead(true);
			setState(Connected);
			connCb(shared_from_this());
		}
		void TcpConn::handleWrite()
		{
			loop_->assertInLoopThread();
			if (channel_->isWriteEnabled())
			{
				assert(!writeBuf.empty());
				// 写writeBuf里面的内容
				ssize_t sended = writeBuf.readOutToFd(channel_->getFd());
				if (sended > 0)
				{
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
				else
				{
					LOG_ERROR("TcpConn::handleWrite");
				}
			}
			else
			{
				LOG("Connection fd=%d is down, no more writing",channel_->getFd());
			}
		}
		void TcpConn::send(const char* buf, size_t len)
		{
			if (state_ == Connected)
			{
				// why not use shared_from_this() ?
				loop_->runInLoop([this,len,buf] {
					sendInLoop(buf, len);
				});
			}
		}
		void TcpConn::send(const std::string& str)
		{
			send(str.data(), str.size());
		}
		void TcpConn::sendInLoop(const std::string& message)
		{
			sendInLoop(message.data(), message.size());
		}
		void TcpConn::sendInLoop(const void* data, size_t len)
		{
			loop_->assertInLoopThread();
			ssize_t sended = 0;
			// 确保写缓冲区没有数据，才能直接写。否则要先把写缓冲区的内容写完才能写新数据，否则会产生乱序
			if (!channel_->isWriteEnabled() && writeBuf.empty())
			{
				sended = sockets::write(channel_->getFd(),data, len);
				if (sended >= 0)
				{
					// 没写完
					if (static_cast<size_t>(sended) < len)
					{
						// 这里没有采取循环写，直到出现EAGAIN。因为如果一次没写完，第二次写几乎肯定会出现EAGAIN。
						// 把剩下没写完的直接丢给handleWrite，多数情况下可以减少一次系统调用 P319
					}
				}
				else
				{
					sended = 0;
					if (errno != EWOULDBLOCK) // EWOULDBLOCK==EAGAIN
					{
						LOG_ERROR("TcpConn::sendInLoop");
					}
				}
			}
			assert(sended >= 0);
			if (static_cast<size_t>(sended) < len)
			{
				writeBuf.writeIn(static_cast<const char*>(data) + sended, len - sended);
				if (!channel_->isWriteEnabled())
				{
					channel_->enableWrite(true);
				}
			}
		}
		std::string TcpConn::getInput()
		{
			return readBuf.readOutAsString(readBuf.size());
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
		// 1. 关闭写端
		// 2. 另一方read==0
		// 3. 对方关闭连接close
		// 4. read==0
		// 5. 我方close
		void TcpConn::shutdownInLoop()
		{
			loop_->assertInLoopThread();
			// TODO : judge channel is writing ?
			// 只关闭写端，要求对端read==0时会断开
			socket_->shutdownWrite();
		}


	} // namespace ryugu
}
