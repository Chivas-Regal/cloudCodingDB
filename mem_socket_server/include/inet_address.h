#pragma once

#include <cstdint>
#include <arpa/inet.h>

class InetAddress {
public:
	InetAddress () noexcept;
	InetAddress (const char* ip, uint16_t port);
	~InetAddress () = default;
public:
	sockaddr_in addr;
	socklen_t addr_len;
};