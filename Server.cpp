#include <poll.h>

#include <iostream>
#include <cassert>
#include <cstring>

#include "Socket.h"
#include "Server.h"
#include "NetCommandsQueue.h"


Server::Server(NetCommandsQueue & queue, unsigned clientTimeout) :
	queue_(queue),
    clients_{},
    pfds_{},
	pfdsCount_(0),
	pollTimeout_(infinite),
	clientsTimeout_(clientTimeout),
	listenSocket_(INVALID_SOCKET)
{
}

Server::~Server()
{
	//todo: stop all
}


void Server::run()
{
    Socket socket;
    socket.create()
          .setReuseAddr()
          .setNoBlockMode()
		  .bind("127.0.0.1", 12000)
          .listen(maxClientsCount);
	listenSocket_ = socket.native();
    clients_[0].attach(std::move(socket));
	pfds_[0].fd = listenSocket_;
    pfds_[0].events = POLLIN;
    ++pfdsCount_;
    while (processPoll()) {
    }
}

bool Server::processPoll()
{
    assert(pfdsCount_ != 0 && "pfds must be great 0");
	if (pollTimeout_ == infinite) {
		pollTimeout_ = 10000;
	}

	int count = poll(pfds_.data(), pfdsCount_, pollTimeout_);

    if (count < 0) {
        throw std::runtime_error(strerror(errno));
    }

	pollTimeout_ = infinite;

    unsigned currentSize = pfdsCount_;
	bool needCompact = false;
    for (unsigned i = 0; i < currentSize; ++i) {
		if (pfds_[i].fd == listenSocket_) {//server socket
			processListenSocket(i);
        } else {
			if (processClientsSocket(i)) {
				needCompact = true;
			}
        }
    }

	if (needCompact) {
		for (unsigned i = 0; i < pfdsCount_; ) {
			if (!clients_[i].socket().valid()) {
				clients_[i] = std::move(clients_[pfdsCount_ - 1]);
				pfds_[i] = pfds_[pfdsCount_ - 1];
				--pfdsCount_;
			} else {
				++i;
			}
		}
	}
	return true;
}

void Server::processListenSocket(unsigned index)
{
	auto & fds = pfds_[index];
	auto & client = clients_[index];
	if (fds.revents == 0) {
		return;
	}
	if ((fds.revents & POLLIN) == 0) {
		throw std::runtime_error("Invalid operation detected");
	}

	while (pfdsCount_ < maxDescriptorsCount)
	{
		Socket newSocket = client.socket().accept();

		if (!newSocket.valid()) {
			break;
		}
		newSocket.setNoBlockMode();
		clients_[pfdsCount_].attach(std::move(newSocket), clientsTimeout_);
		std::cout << "accept success" << std::endl;
		pfds_[pfdsCount_].fd = clients_[pfdsCount_].socket().native();
		pfds_[pfdsCount_].events = POLLIN;
		++pfdsCount_;
	}
}

bool Server::processClientsSocket(unsigned index)
{
	auto & fds = pfds_[index];
	auto & client = clients_[index];

	bool needDisconnect = true;
	if (fds.revents & POLLIN) {
		if (client.read() && client.buffer().size() < maxBytesOnClient)
		{
			if (PacketDispatcher().extractFullPacket(client, packet_)) {
				std::cout << "packet: " << packet_ << std::endl;
				queue_.push(packet_);
			} else {
				needDisconnect = false;
				std::cout << "buffer: " << client.buffer() << std::endl;
			}
		}
	}
	unsigned lost = client.lost();

	if (lost == 0) {
		needDisconnect = true;
	} else if (lost < pollTimeout_) {
		pollTimeout_ = lost;
	}

	if (needDisconnect) {
		std::cout << "disconnected: " << client.socket().native() << std::endl;
		client.disconnect();
	}

	return needDisconnect;
}
