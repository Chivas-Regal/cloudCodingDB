#pragma once

#include "inet_address.h"
#include <sys/syslimits.h>

/**
 * @brief socket套接字封装类
 * 
 * @details 与 InetAddress 网络地址类一起使用，简化了几个内部函数
 */
class Socket {
public:
    /* 构造：默认初始化一个 ipv4 ，面向字节流的 socket 套接字 */
	Socket ();

    /* 构造：显式传参套接字构造 */
	explicit Socket (int _fd);

    /* 析构：socket 关闭函数 */
	~Socket ();

    /* 绑定一个地址 */
	void bind  	 (InetAddress* addr);

    /* 从全连接队列取出一个套接字 */
	int accept   (InetAddress* addr);

    /* 客户端用：连接服务端地址 */
	void connect (InetAddress* addr);

    /* 服务端开启监听(半连接队列最大长度) */
	void listen  (int size = MAX_CANON) const;

    /* 设置非阻塞IO */
    void setnonblocking ();

    /* 获取套接字 */
	[[nodiscard]] int getFd () const noexcept;
private:
	int fd;
};