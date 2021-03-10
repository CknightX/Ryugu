/**
 * TcpConnection
 * Tcp连接
 */

#pragma once
#include <memory>
#include <string>
#include <cstring>
#include "Utils.h"
#include "Net.h"
#include "Buffer.h"
#include "Callbacks.h"
#include "InetAddr.h"
#include "Socket.h"



namespace ryugu
{
	class EventLoop;
	class Channel;
	class TcpConn;


	class TcpConn : public std::enable_shared_from_this<TcpConn>, noncopyable
	{
	public:
		// Tcp 连接的状态
		enum State
		{
			Invalid = 1,
			Handshaking,  //握手
			Connecting,    // 连接
			Disconnected,
			Closed,
			Failed,
		};

		EventLoop* loop_;
		std::unique_ptr<Channel> channel_;
		std::unique_ptr<net::Socket> socket_;
		State state_;
		net::InetAddr localAddr_, peerAddr_;
		// 使用者定义的回调函数
		TcpCallBack readCb, writeCb, connCb, closeCb;

		Buffer readBuf, writeBuf;

	public:
		TcpConn(EventLoop* loop, int sockfd, net::InetAddr localAddr, net::InetAddr peerAddr);
		~TcpConn();

		// 绑定至channel的回调函数
		void handleRead();
		void handleWrite();

		// 自用
		void handleClose();

		void setState(State _state);

		// 设置读写回调函数
		void setReadCb(const TcpCallBack& cb) { readCb = cb; }
		void setWriteCb(const TcpCallBack& cb) { writeCb = cb; }
		void setConnCb(const TcpCallBack& cb) { connCb = cb; }
		void setCloseCb(const TcpCallBack& cb) { closeCb = cb; }

		// 发起连接(作为客户端时)
		void connect(EventLoop* _loop, const std::string& host, unsigned short port, int timeout, const std::string& localip);
		void reconnect();


		// 发送数据
		void send(const std::string& str);
		void send(const char* buf, size_t len);
		void send(const char* s) { send(s, strlen(s)); }

		// 连接建立时执行
		void connectEstablished();
		// 连接被TcpServer移除时执行
		void connectDestroyed();

		// 将当前readBuf中的所有数据以string返回
		std::string getInput();
		EventLoop* getLoop() const { return loop_; }
		int getFd()const;




	private:
		// 尝试尽可能发送，返回最终发送的字节数
		size_t _write(const char* buf, size_t len);
		// 尽可能从读取到readBuf中
		void fillReadBuf();
	};

} // namespace ryugu
