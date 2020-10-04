#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <stdexcept>
#include <cstring>
#include "Socket.h"


Socket::Socket():
    socket_(INVALID_SOCKET)
{
}

Socket::Socket(SOCKET socket):
    socket_(socket)
{
}

Socket::Socket(Socket && src):
    socket_(std::exchange(src.socket_, INVALID_SOCKET))
{
}

Socket & Socket::operator=(Socket && src)
{
    if (this != &src) {
        socket_ = std::exchange(src.socket_, socket_);
        src.close();
    }
    return *this;
}


Socket::~Socket()
{
    close();
}

void Socket::swap(Socket & src)
{
    std::swap(socket_, src.socket_);
}

bool Socket::valid()
{
    return socket_ != INVALID_SOCKET;
}

Socket & Socket::create()
{
    close();
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        throw std::runtime_error(strerror(errno));
    }
    return *this;
}

Socket & Socket::setReuseAddr()
{
	int on = 1;
    int rc = setsockopt(socket_, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0) {
        throw std::runtime_error(strerror(errno));
    }
    return *this;
}

Socket & Socket::setNoBlockMode()
{
	int on = 1;
	int rc = ioctl(socket_, FIONBIO, &on);
    if (rc < 0) {
        throw std::runtime_error(strerror(errno));
    }
    return *this;
}

Socket & Socket::bind(const char * ipStr, uint16_t port)
{
    unsigned long ip = inet_addr (ipStr);
    if (ip == INADDR_NONE) {
        throw std::invalid_argument(strerror(errno));
    }
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip;
    int rc = ::bind(socket_, (sockaddr *)&addr, sizeof(addr));
    if (rc < 0) {
        throw std::runtime_error(strerror(errno));
    }
    return *this;
}

Socket & Socket::listen(int backlog)
{
    int rc = ::listen(socket_, backlog);
    if (rc < 0) {
        throw std::runtime_error(strerror(errno));
    }
    return *this;
    return *this;
}

Socket Socket::accept()
{
    return ::accept(socket_, NULL, NULL);
}

void Socket::close()
{
    if (socket_ != INVALID_SOCKET) {
		::shutdown(socket_, SHUT_RDWR);
		::close(socket_);
        socket_ = INVALID_SOCKET;
    }
}

SOCKET Socket::native()
{
    return socket_;
}
