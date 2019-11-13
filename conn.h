/**
 * TcpConnection
 * 对每一个Tcp连接进行抽象
 */

#pragma once
#include <memory>
#include <string>
#include <cstring>
#include "utils.h"
#include "net.h"
#include "buffer.h"



namespace ck
{
class EventHandler;
class Channel;
class TcpConn;

using TcpConnPtr = std::shared_ptr<TcpConn>;
using TcpCallBack = std::function<void(const TcpConnPtr &)>;

class TcpConn : public std::enable_shared_from_this<TcpConn>, noncopyable
{
public:
    // Tcp 连接的状态
    enum State
    {
        Invalid = 1,
        Handshaking,  //握手阶段
        Connected,    // 连接
        Closed,
        Failed,
    };

    EventHandler *handler;
    Channel *channel;
    State state;
    Ipv4Addr local,peer;
    // 使用者定义的回调函数
    TcpCallBack readcb, writecb;

    Buffer readBuf,writeBuf;

public:
    TcpConn();
    virtual ~TcpConn(){}
    // 可读可写时的回调函数
    // 用户设置的行为只是其中一部分，其中将数据读取到缓冲区的行为由库自动完成
    // 写也是同理
    void handleRead(const TcpConnPtr &conn);
    void handleWrite(const TcpConnPtr &conn);

    // 设置读写回调函数
    void setReadCB(const TcpCallBack &cb){readcb=cb;}
    void setWriteCB(const TcpCallBack &cb){writecb=cb;}

    // 发起连接(作为客户端时)
    void connect(EventHandler* _handler, const std::string &host, unsigned short port, int timeout, const std::string &localip);
    void reconnect();

    // 关联一个fd
    void attach(EventHandler* _handler, int fd, Ipv4Addr _local, Ipv4Addr _peer);

    // 发送数据
    void send(const std::string& str);
    void send(const char* buf,size_t len);
    void send(const char* s){send(s,strlen(s));}

    std::string getInput();




    template <typename C = TcpConnPtr>
    static TcpConnPtr createConnection(EventHandler *base, const std::string &host, unsigned short port, int timeout = 0, const std::string &localip = "")
    {
        TcpConnPtr conn(new C);
        conn->connect(base, host, port, timeout, localip);
        return conn;
    }

    template <typename C = TcpConn>
    static TcpConnPtr createConnection(EventHandler *_handler, int fd, Ipv4Addr local, Ipv4Addr peer)
    {
        TcpConnPtr conn(new C);
        conn->attach(_handler, fd, local, peer);
        return conn;
    }
    private:
    // 封装系统调用write，尝试尽可能写，返回实际写的字节数
    size_t _write(const char* buf,size_t len);
};

} // namespace ck