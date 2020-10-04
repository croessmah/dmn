#include <sys/types.h>
#include <sys/socket.h>

#include "Client.h"


Client::Client()
{
}

Client::Client(Client && src):
    socket_(std::move(src.socket_)),
    buffer_(src.buffer_),
	attachTime_(src.attachTime_),
	timeout_(src.timeout_)
{
}

Client & Client::operator=(Client && src)
{
    if (this != & src) {
        disconnect();
        socket_.swap(src.socket_);
        buffer_.swap(src.buffer_);
		std::swap(attachTime_, src.attachTime_);
		std::swap(timeout_, src.timeout_);
    }
    return *this;
}

Client::~Client()
{
    disconnect();
}

bool Client::connected()
{
    return socket_.valid();
}

void Client::swap(Client & src)
{
    if (this != &src) {
        socket_.swap(src.socket_);
        buffer_.swap(src.buffer_);
		std::swap(attachTime_, src.attachTime_);
		std::swap(timeout_, src.timeout_);
    }
}

void Client::attach(Socket && socket, unsigned timeout)
{
    socket_ = std::move(socket);
    buffer_.clear();
	attachTime_ = Clock::now();
	timeout_ = timeout;
}

bool Client::read()
{
    char buffer[4096];

    if (!socket_.valid()) {
        return false;
    }
    int rc = recv(socket_.native(), buffer, sizeof(buffer), 0);
    if (rc == 0) {
        return false;
    }

    do {
        if (rc < 0) {
            return errno == EWOULDBLOCK || errno == EAGAIN;
        }
        buffer_.append(buffer, rc);
        rc = recv(socket_.native(), buffer, sizeof(buffer), 0);
    } while (rc != 0);
    return true;
}

unsigned Client::elapsed()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - attachTime_).count();
}

unsigned Client::lost()
{
	unsigned elapsedMs = elapsed();
	if (timeout_ < elapsedMs) {
		return 0;
	}
	if (timeout_ == infinite) {
		return timeout_;
	}
	return timeout_ - elapsedMs;
}

std::string const & Client::buffer()
{
    return buffer_;
}

bool Client::extract(size_t count)
{
    if (count < buffer_.size()) {
        buffer_.erase(0, count);
    } else {
        buffer_.clear();
    }
    return !buffer_.empty();
}

void Client::disconnect()
{
    socket_.close();
    buffer_.clear();
}

Socket & Client::socket()
{
    return socket_;
}
