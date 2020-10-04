#ifndef SOCKET_H
#define SOCKET_H

#include <WinSock2.h>
#include <Windows.h>
#include <cstdint>

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
    void close();
    SOCKET native();
private:
    SOCKET socket_;
};

#endif // SOCKET_H
