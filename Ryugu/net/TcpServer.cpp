#include <cstring>
#include <assert.h>
#include "Ryugu/net/TcpServer.h"
#include "Ryugu/net/EventLoop.h"
#include "Ryugu/net/Channel.h"
#include "Ryugu/net/SocketsOps.h"
#include "Ryugu/net/Acceptor.h"
#include "Ryugu/net/EventLoopThreadPool.h"
#include "Ryugu/net/TcpConn.h"
#include "Ryugu/base/log/Logging.h"
namespace ryugu
{
	namespace net
	{
		TcpServer::TcpServer(EventLoop* loop, const net::InetAddr& listenAddr, bool reusePort)
			:loop_(loop),
			acceptor_(new Acceptor(loop, listenAddr, reusePort)),
			threadPool_(new EventLoopThreadPool(loop, "test")),
			connCb(nullptr),
			messageCb(nullptr)

		{
			acceptor_->setNewConnCallback([this](int sockfd, const InetAddr& peerAddr) {
				newConnection(sockfd, peerAddr);
			});
			LOG_INFO << "TcpServer created, address:" << listenAddr.getIpPortStr();
		}
		TcpServer::~TcpServer()
		{
			// 主线程中
			loop_->assertInLoopThread();
			LOG_DEBUG << "TcpServer destruct";
			for (auto& item : connMap_)
			{
				auto connPtr = item.second;
				item.second->getLoop()->runInLoop([connPtr] {
					connPtr->connectDestroyed();
				});
			}
		}
		void TcpServer::newConnection(int sockfd, const InetAddr& peerAddr)
		{
			// 在主线程中执行
			loop_->assertInLoopThread();
			auto localAddr = net::InetAddr(net::sockets::getLocalAddr(sockfd));
			EventLoop* ioLoop = threadPool_->getOneLoop();
			TcpConnPtr newConn(new TcpConn(ioLoop, sockfd, localAddr, peerAddr));
			// 加入客户连接池
			connMap_[sockfd] = newConn;
			// 设置回调
			newConn->setConnCb(connCb);
			newConn->setReadCb(messageCb);
			newConn->setCloseCb([this](const TcpConnPtr& conn) {
				removeConnection(conn);
			});
			// IO线程调用establish->channel纳入poller中开始事件循环
			// 包含对于io线程中poller的update，所以要放到io线程中执行
			ioLoop->runInLoop([newConn] {
				newConn->connectEstablished();
			});
		}
		void TcpServer::start()
		{
			threadPool_->start();
			assert(!acceptor_->isListening());

			loop_->runInLoop([this] {
				acceptor_->listen();
			});

		}
		void TcpServer::setThreadNum(int num)
		{
			threadPool_->setThreadNum(num);
		}
		void TcpServer::removeConnection(const TcpConnPtr& conn)
		{
			// 对connMap操作需要在主进程
			// 必须值捕获conn，二次引用绑定无法继续延长临时变量生命期
			loop_->runInLoop([this, conn] {
				removeConnectionInLoop(conn);
			});
		}
		void TcpServer::removeConnectionInLoop(const TcpConnPtr& conn)
		{
			LOG_DEBUG << "removeConnectionInLoop.";
			connMap_.erase(conn->getFd());
			auto ioLoop = conn->getLoop();
			// 同上
			ioLoop->queueInLoop([this, conn] {
				conn->connectDestroyed();
			});
		}
	} 
}
