#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <ctime>
#include <chrono>
#include "Socket.h"



struct Client
{
public:
	static constexpr unsigned infinite = ~0;
	using Clock = std::chrono::high_resolution_clock;
	using Timepoint = Clock::time_point;
    Client();
    Client(Client && src);
    Client & operator=(Client && src);
    Client(Client const & ) = delete;
    Client & operator=(Client const & ) = delete;
    ~Client();
    bool connected();
    void swap(Client & src);
	void attach(Socket && socket, unsigned timeout = infinite);
    bool read();
	unsigned elapsed();
	unsigned lost();
    std::string const & buffer();
    bool extract(size_t count);
    void disconnect();
    Socket & socket();
private:
    Socket socket_;
    std::string buffer_;
	Timepoint attachTime_;
	unsigned timeout_;
};




#endif // CLIENT_H
