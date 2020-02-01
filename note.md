文件结构
channel.h     : 对fd进行封装，是读写的通道。可以放到poller里面复用
conn.h        : TcpConnction类，是对一个Tcp连接的抽象封装
net.h         : 定义了网络相关的对象
tcpserver.h   : tcp服务端实现
poller        : 封装了底层的IO复用机制提供统一接口 


EventHandler 对于各个组件的管理是通过在各个组件里面放一个EventHandler*来实现的


TODO:
1. 定时器实现 ok
1. Tcp 客户端
1. Udp 客户端服务端
1. 多线程服务器