#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include <Windows.h>


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


class Server
{
public:
    static constexpr unsigned infinite = ~0u;
    Server(unsigned clientTimeout = infinite);
    ~Server();
    void run();
private:
    void pushServerSocket();
    bool acceptClients();
    bool processPoll();
    static constexpr unsigned maxClientsCount = 64;
    static constexpr unsigned maxBytesOnClient = 4 * 1024 - 1; //4 Kb - 1 byte for null
    std::array<Client, maxClientsCount + 1> clients_;
    std::array<WSAPOLLFD, maxClientsCount + 1> pfds_;
    std::string packet_;
    unsigned pfdsCount_;
    unsigned minTimeout;
    unsigned timeout;
    SOCKET listenSocket;
};

#endif // SERVER_H
