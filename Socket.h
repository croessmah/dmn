#ifndef SOCKET_H
#define SOCKET_H

#include <cstdint>

using SOCKET = int;
constexpr int INVALID_SOCKET = -1;


class Socket
{
public:
    Socket();
    Socket(SOCKET socket);
    Socket(Socket && src);
    Socket & operator=(Socket && src);
    Socket(Socket const &) = delete;
    Socket & operator=(Socket const &) = delete;
    ~Socket();
    void swap(Socket & src);
    bool valid();
    Socket & create();
    Socket & setReuseAddr();
    Socket & setNoBlockMode();
    Socket & bind(char const * ipStr, uint16_t port);
    Socket & listen(int backlog);
    Socket accept();
	bool connect(char const * ipStr, uint16_t port);
    void close();
    SOCKET native();
private:
    SOCKET socket_;
};

#endif // SOCKET_H
