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



namespace ck
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
        Connected,    // 连接
        Closed,
        Failed,
    };

    EventLoop *loop;
    Channel *channel;
    State state;
    net::Ipv4Addr local,peer;
    // 使用者定义的回调函数
    TcpCallBack readCb, writeCb, connCb;

    Buffer readBuf,writeBuf;

public:
    TcpConn();
    virtual ~TcpConn();
    // 可读可写时的回调函数
    // 用户设置的行为只是其中一部分，其中将数据读取到缓冲区的行为由库自动完成
    // 写也是同理
    void handleRead(const TcpConnPtr &conn);
    void handleWrite(const TcpConnPtr &conn);

    void setState(State _state);

    // 设置读写回调函数
    void setReadCb(const TcpCallBack &cb){readCb=cb;}
    void setWriteCb(const TcpCallBack &cb){writeCb=cb;}
    void setConnCb(const TcpCallBack &cb){connCb=cb;}

    // 发起连接(作为客户端时)
    void connect(EventLoop* _loop, const std::string &host, unsigned short port, int timeout, const std::string &localip);
    void reconnect();

    // 关联一个fd
    void attach(EventLoop* _loop, int fd, net::Ipv4Addr _local, net::Ipv4Addr _peer);

    // 发送数据
    void send(const std::string& str);
    void send(const char* buf,size_t len);
    void send(const char* s){send(s,strlen(s));}

    // 将当前readBuf中的所有数据以string返回
    std::string getInput() ;

    void close();




    template <typename C = TcpConnPtr>
    static TcpConnPtr createConnection(EventLoop *base, const std::string &host, unsigned short port, int timeout = 0, const std::string &localip = "")
    {
        TcpConnPtr conn(new C);
        conn->connect(base, host, port, timeout, localip);
        return conn;
    }

    template <typename C = TcpConn>
    static TcpConnPtr createConnection(EventLoop *_loop, int fd, net::Ipv4Addr local, net::Ipv4Addr peer)
    {
        TcpConnPtr conn(new C);
        conn->attach(_loop, fd, local, peer);
        return conn;
    }
    private:

    // 尝试尽可能发送，返回最终发送的字节数
    size_t _write(const char* buf,size_t len);
    // 尽可能从读取到readBuf中
    void fillReadBuf();
};

} // namespace ck
