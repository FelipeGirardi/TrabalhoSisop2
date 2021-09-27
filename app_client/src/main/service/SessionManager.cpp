/**
 * Thread-safe manager for a single user session.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "service/SessionManager.hpp"
#include "io/CommandLineParser.hpp"
#include "exception/HostNotFoundException.hpp"
#include "exception/SocketNotCreatedException.hpp"
#include "exception/AuthenticationFailedException.hpp"
#include "exception/SocketConnectionDeniedException.hpp"
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace ClientApp::Service;
using namespace std;

using ClientApp::Exception::HostNotFoundException;
using ClientApp::Exception::SocketNotCreatedException;
using ClientApp::Exception::AuthenticationFailedException;
using ClientApp::Exception::SocketConnectionDeniedException;
using ClientApp::IO::Socket;
using ClientApp::IO::CommandLineParser;

// Static variables

bool SessionManager::isConnected_ = false;
SessionSockets* SessionManager::sockets_ = nullptr;
mutex SessionManager::managerMutex_;

// Public methods

SessionSockets SessionManager::connect(string profileId, string host, int port)
{
    managerMutex_.lock();

    if (!isConnected_)
    {
        auto hostEntry = createHostEntry(host);

        auto senderSocketDescritor = createSocketDescriptor();
        auto listenerSocketDescritor = createSocketDescriptor();

        auto senderSocket = connectSocketToServer(senderSocketDescritor, hostEntry, port);
        auto listenerSocket = connectSocketToServer(listenerSocketDescritor, hostEntry, port);
        sockets_ = new SessionSockets{ senderSocket, listenerSocket };

        authenticateProfile(profileId);
        isConnected_ = true;
    }

    managerMutex_.unlock();
    return *sockets_;
}

void SessionManager::disconnect()
{
    managerMutex_.lock();

    Packet exitPacket = { EXIT, 256, 0, "EXIT" };
    sockets_->senderSocket.send(exitPacket);

    ::close(sockets_->listenerSocket.getDescriptor());
    ::close(sockets_->senderSocket.getDescriptor());
    isConnected_ = false;

    delete sockets_;
    sockets_ = nullptr;

    managerMutex_.unlock();
}


// Private methods

hostent* SessionManager::createHostEntry(std::string host)
{
    auto hostEntry = gethostbyname(host.c_str());
    if (hostEntry == nullptr)
        throw HostNotFoundException(host);

    return hostEntry;
}

int SessionManager::createSocketDescriptor()
{
    auto socketDescriptor = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor == -1)
        throw SocketNotCreatedException();

    return socketDescriptor;
}

Socket SessionManager::connectSocketToServer(int socketDescriptor, const hostent* hostEntry, int port)
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

void SessionManager::authenticateProfile(std::string profileId)
{
    try
    {
        auto standardProfileId = CommandLineParser::standardizeProfileId(profileId);
        sockets_->senderSocket.send(standardProfileId.c_str());
    }
    catch (...)
    {
        throw AuthenticationFailedException(profileId);
    }
}
