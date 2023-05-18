#pragma once

#include <cstdint>
#include <arpa/inet.h>

/**
 * @brief 网络地址类
 * 
 * @details 主存放信息，被其他类使用
 *          ip:port
 */
class InetAddress {
public:
    /* 不设置 addr ，只设置 addr_len = sizeof(addr) */
	InetAddress () noexcept;
    /* addr 设置 ipv4 ，ip:port */
	InetAddress (const char* ip, uint16_t port);
    
	~InetAddress () = default;
public:
	sockaddr_in addr;   ///< 地址类
	socklen_t addr_len; ///< 类长度
};