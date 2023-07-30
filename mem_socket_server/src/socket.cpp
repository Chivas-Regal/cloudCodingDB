#include "socket.h"
#include "utils.h"
#include "limits.h"
#include <sys/socket.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * @brief 默认构造函数
 * 
 * @details 默认初始化一个 ipv4 ，面向字节流的 socket 套接字
 */
Socket::Socket() :
	fd(socket(AF_INET, SOCK_STREAM, 0))
{
	errif(fd == -1, "error socket create");
}

/**
 * @brief 显式套接字构造函数
 * 
 * @param _fd 已有套接字
 */
Socket::Socket(int _fd) :
	fd(_fd)
{
	errif(fd == -1, "error socket create");
}

/**
 * @brief socket 关闭函数
 * 
 */
Socket::~Socket()
{
	if (~fd) {
		close(fd);
		fd = -1;
	}
}

/**
 * @brief 绑定一个地址
 * 
 * @param addr 被绑定地址
 */
void Socket::bind(InetAddress *addr) {
	errif(
		::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1,
		"error socket bind"
	);
}

/**
 * @brief 从全连接队列取出一个套接字
 * 
 * @param[out] addr 取出后修改为对方地址
 * @return int      返回取出的套接字
 */
int Socket::accept(InetAddress *addr) {
	int acceptfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
	errif (
		acceptfd == -1,
		"error socket accept"
	);
	return acceptfd;
}

/**
 * @brief 客户端用：连接服务端地址
 * 
 * @param addr 服务端地址
 */
void Socket::connect(InetAddress *addr) {
	errif(
		::connect(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1,
		"error socket connect"
	);
}

/**
 * @brief 服务端开启监听
 * 
 * @param size 半连接队列长度
 * 
 * @details 半连接队列默认 1024
 */
void Socket::listen(int size) const {
	errif (
		::listen(fd, MAX_CANON) == -1,
		"error socket listen"
	);
}

/**
 * @brief 设置非阻塞IO
 * 
 */
void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

/**
 * @brief 获取套接字
 * 
 * @return int 套接字
 */
int Socket::getFd() const noexcept {
	return fd;
}
