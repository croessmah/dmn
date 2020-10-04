#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <WinSock2.h>
#include <Windows.h>

#include <iostream>
#include <cassert>
#include "Socket.h"
#include "Server.h"


Server::Server(unsigned clientTimeout) :
    clients_{},
    pfds_{},
    pfdsCount_(0),
    timeout(clientTimeout),
    listenSocket(INVALID_SOCKET)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
}

Server::~Server()
{
    WSACleanup();
}


void Server::run()
{
    Socket socket;
    socket.create()
          .setReuseAddr()
          .setNoBlockMode()
          .bind("127.0.0.1", 12000)
          .listen(maxClientsCount);
    listenSocket = socket.native();
    clients_[0].attach(std::move(socket));
    pfds_[0].fd = listenSocket;
    pfds_[0].events = POLLIN;
    ++pfdsCount_;
    while (processPoll()) {
    }
}

bool Server::processPoll()
{
    assert(pfdsCount_ != 0 && "pfds must be great 0");
    int count = WSAPoll(pfds_.data(), pfdsCount_, minTimeout);
    if (count < 0) {
        throw std::runtime_error(strerror(errno));
    }

    if (count == 0) {
        //todo: timeout
    }

    unsigned currentSize = pfdsCount_;
    for (unsigned i = 0; i < currentSize; ++i) {
        auto & fds = pfds_[i];
        auto & client = clients_[i];
        std::clock_t now = std::clock();
        if(fds.revents == 0) {
            if (fds.fd != listenSocket
                && timeout != infinite
                && timeout < unsigned(now - client.updateClock())) {
                client.disconnect();
            }
            continue;
        }

        if (fds.fd == listenSocket) {//server socket
            if ((fds.revents & POLLIN) == 0) {
                throw std::runtime_error("Invalid operation detected");
            }
            if (pfdsCount_ != (maxClientsCount + 1)) {
                clients_[pfdsCount_].attach(client.socket().accept());
                if (clients_[pfdsCount_].socket().valid()) {
                    std::cout << "accept success" << std::endl;
                    pfds_[pfdsCount_].fd = clients_[pfdsCount_].socket().native();
                    pfds_[pfdsCount_].events = POLLIN;
                    ++pfdsCount_;
                } else {
                    std::cout << "accept failure" << std::endl;
                }
            }
        } else {
            if ((fds.revents & POLLIN) == 0
                || !client.read()
                || client.buffer().size() > maxBytesOnClient ) {
                std::cout << "disconnected: " << client.socket().native() << std::endl;
                client.disconnect();
            } else {
                if (PacketDispatcher().extractFullPacket(client, packet_)) {
                    std::cout << "packet: " << packet_ << std::endl;
                    client.disconnect();
                } else {
                    std::cout << "buffer: " << client.buffer() << std::endl;
                }
            }
        }
    }

    currentSize = pfdsCount_;
    for (unsigned i = 0; i < currentSize; ++i) {
        if (!clients_[i].socket().valid()) {
            //todo: compact
        }
    }


    return true;
}
