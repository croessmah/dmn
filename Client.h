#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <ctime>
#include "Socket.h"



struct Client
{
public:
    Client();
    Client(Client && src);
    Client & operator=(Client && src);
    Client(Client const & ) = delete;
    Client & operator=(Client const & ) = delete;
    ~Client();
    bool connected();
    void swap(Client & src);
    void attach(Socket && socket);
    bool read();
    std::clock_t updateClock();
    std::string const & buffer();
    bool extract(size_t count);
    void disconnect();
    Socket & socket();
private:
    Socket socket_;
    std::string buffer_;
    std::clock_t clock_;
};




#endif // CLIENT_H
