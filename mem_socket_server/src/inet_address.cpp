#include "inet_address.h"
#include <string>
#include <cstring>

/**
 * @brief 默认构造函数
 * 
 * @details 不设置 addr ，只设置 addr_len = sizeof(addr)
 */
InetAddress::InetAddress() noexcept:
	addr_len(sizeof(addr))
{
	bzero(&addr, sizeof(addr));
}

/**
 * @brief 以 ip:port 初始化地址类
 * 
 * @param ip   ip地址
 * @param port 端口
 * 
 * @details ipv4地址族
 */
InetAddress::InetAddress(const char *ip, uint16_t port) :
	addr_len(sizeof(addr))
{
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	addr_len = sizeof(addr);
}
