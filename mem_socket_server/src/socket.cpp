#include "../include/socket.h"
#include "../include/util.h"
#include "../include/limits.h"
#include <sys/socket.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket() :
	fd(socket(AF_INET, SOCK_STREAM, 0))
{
	errif(fd == -1, "error socket create");
}

Socket::Socket(int _fd) :
	fd(_fd)
{
	errif(fd == -1, "error socket create");
}

Socket::~Socket()
{
	if (~fd) {
		close(fd);
		fd = -1;
	}
}

void Socket::bind(InetAddress *addr) {
	errif(
		::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1,
		"error socket bind"
	);
}

int Socket::accept(InetAddress *addr) {
	int acceptfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
	errif (
		acceptfd == -1,
		"error socket accept"
	);
	return acceptfd;
}

void Socket::connect(InetAddress *addr) {
	errif(
		::connect(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1,
		"error socket connect"
	);
}

void Socket::listen() const {
	errif (
		::listen(fd, MAX_CANON) == -1,
		"error socket listen"
	);
}

void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::getFd() const noexcept {
	return fd;
}
