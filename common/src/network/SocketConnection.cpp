/**
 * Functions to support connecting sockets.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "common/network/SocketConnection.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;
using namespace Common::Model;
using namespace Common::Network;
using namespace Common::Exception;

// Free functions
hostent* createHostEntry(std::string host)
{
    auto hostEntry = ::gethostbyname(host.c_str());
    if (hostEntry == nullptr)
        throw HostNotFoundException(host);

    return hostEntry;
}

int createSocketDescriptor()
{
    auto socketDescriptor = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor == -1)
        throw SocketNotCreatedException();

    return socketDescriptor;
}

Socket connectSocketToHost(int socketDescriptor, const hostent* hostEntry, int port)
{
    struct sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(port);
    socketAddress.sin_addr = *((struct in_addr*)hostEntry->h_addr);
    bzero(&(socketAddress.sin_zero), 8);

    auto response = ::connect(socketDescriptor, (struct sockaddr*)&socketAddress, sizeof(socketAddress));
    if (response < 0)
        throw SocketConnectionDeniedException();

    return Socket(socketDescriptor);
}

void closeSocket(int socketDescriptor)
{
    ::close(socketDescriptor);
}
