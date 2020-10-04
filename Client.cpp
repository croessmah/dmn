#include "Client.h"


Client::Client()
{
}

Client::Client(Client && src):
    socket_(std::move(src.socket_)),
    buffer_(src.buffer_),
    clock_(src.clock_)
{
}

Client & Client::operator=(Client && src)
{
    if (this != & src) {
        disconnect();
        socket_.swap(src.socket_);
        buffer_.swap(src.buffer_);
        std::swap(clock_, src.clock_);
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
        std::swap(clock_, src.clock_);
    }
}

void Client::attach(Socket && socket)
{
    socket_ = std::move(socket);
    buffer_.clear();
    clock_ = std::clock();
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
    clock_ = std::clock();
    do {
        if (rc < 0) {
            return errno == EWOULDBLOCK || errno == EAGAIN;
        }
        buffer_.append(buffer, rc);
        rc = recv(socket_.native(), buffer, sizeof(buffer), 0);
    } while (rc != 0);
    return true;
}

clock_t Client::updateClock()
{
    return clock_;
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
