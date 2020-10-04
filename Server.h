#ifndef SERVER_H
#define SERVER_H

#include <sys/poll.h>

#include <array>
#include <string>
#include <ctime>
#include <cstdint>

#include "Socket.h"
#include "Client.h"


class PacketDispatcher
{
public:
    bool extractFullPacket(Client & client, std::string & result)
    {
        size_t endPos = client.buffer().find('\0');
        if (endPos == std::string::npos) {
            return false;
        }
        result.clear();
        result.append(client.buffer().data(), endPos + 1);
        client.extract(endPos + 1);
        return true;
    }
};

class NetCommandsQueue;


class Server
{
public:
    static constexpr unsigned infinite = ~0u;
	Server(NetCommandsQueue & queue, unsigned clientTimeout = infinite);
    ~Server();
    void run();
private:
    void pushServerSocket();
    bool acceptClients();
    bool processPoll();
	void processListenSocket(unsigned index);
	bool processClientsSocket(unsigned index);
    static constexpr unsigned maxClientsCount = 64;
    static constexpr unsigned maxBytesOnClient = 4 * 1024 - 1; //4 Kb - 1 byte for null
	static constexpr unsigned maxDescriptorsCount = maxClientsCount + 1;
	NetCommandsQueue & queue_;
	std::array<Client, maxDescriptorsCount> clients_;
	std::array<pollfd, maxDescriptorsCount> pfds_;
    std::string packet_;
    unsigned pfdsCount_;
	unsigned pollTimeout_;
	unsigned clientsTimeout_;
	SOCKET listenSocket_;
};

#endif // SERVER_H
