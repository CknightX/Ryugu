#include "Ryugu/net/Acceptor.h"
#include "Ryugu/net/EventLoop.h"
#include "Ryugu/net/Socket.h"
#include "Ryugu/net/Channel.h"
#include "Ryugu/net/SocketsOps.h"
#include "Ryugu/net/InetAddr.h"
namespace ryugu
{
	namespace net
	{
		Acceptor::Acceptor(EventLoop* loop, const InetAddr& listenAddr, bool reusePort)
			:loop_(loop),
			acceptSocket_(new Socket(sockets::createNonblockingSocket(listenAddr.family()))),
			acceptChannel_(new Channel(loop,acceptSocket_->getFd())),
			listening_(false)
		{
			acceptSocket_->setReuseAddr(true);
			acceptSocket_->setReusePort(reusePort);
			acceptSocket_->bind(listenAddr);
			acceptChannel_->setReadCB([this] {
				handleRead();
			});
		}
		Acceptor::~Acceptor()
		{
			acceptChannel_->enableAll(false);
			acceptChannel_->remove();
		}
		void Acceptor::listen()
		{
			loop_->assertInLoopThread();
			listening_ = true;
			acceptSocket_->listen();
			acceptChannel_->enableRead(true);
		}
		void Acceptor::handleRead()
		{
			InetAddr peerAddr;
			int connfd = acceptSocket_->accept(&peerAddr);
			if (connfd >= 0)
			{
				if (newConnCb_)
				{
					newConnCb_(connfd, peerAddr);
				}
				else
				{
					sockets::close(connfd);
				}
			}
			else
			{
				LOG_ERROR("Acceptor::handleRead");
			}
		}
	}
}