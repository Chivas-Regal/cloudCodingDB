#pragma once

#include "inet_address.h"

class Socket {
public:
	Socket ();
	explicit Socket (int _fd);
	~Socket ();

	void bind  	 (InetAddress* addr);
	int accept   (InetAddress* addr);
	void connect (InetAddress* addr);
	void listen  () const;

    void setnonblocking ();

	[[nodiscard]] int getFd () const noexcept;
private:
	int fd;
};