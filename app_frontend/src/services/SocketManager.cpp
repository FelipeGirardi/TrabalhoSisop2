/**
 * Manage sockets.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "services/SocketManager.hpp"
#include "exceptions/SocketNotCreatedException.hpp"
#include "exceptions/SocketBindFailedException.hpp"
#include "exceptions/SocketAcceptFailedException.hpp"
#include "exceptions/SocketListenFailedException.hpp"
#include "exceptions/UnexpectedPacketTypeException.hpp"

#include <thread>
#include <memory>
#include <cstring>
#include <iostream>
#include <exception>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace Common;
using namespace FrontendApp::IO;
using namespace FrontendApp::Service;
using namespace FrontendApp::Exception;

// Static variables

mutex SocketManager::serverMutex_;
ServerSession serverSession_ = { nullptr, nullptr };
std::unordered_map<std::string, std::list<ClientSession>> clientSessions_;


// Public methods

void SocketManager::listenToServerChanges(std::string host, int port)
{
    auto listenerSocketDescriptor = createSocketDescriptor();
    initializeListenerSocket(listenerSocketDescriptor, host, port);

    while (true)
    {
        sockaddr_in serverAddress;
        socklen_t serverLength;

        auto firstServerSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&serverAddress, &serverLength);
        if (firstServerSocketDescriptor < 0)
            throw SocketAcceptFailedException(listenerSocketDescriptor, false);

        serverMutex_.lock();

        serverSession_.commandSocket = nullptr;
        serverSession_.notificationSocket = nullptr;

        auto firstServerSocket = make_shared<Socket>(new Socket(firstServerSocketDescriptor));
        identifyServerSocketType(firstServerSocket);

        auto secondServerSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&serverAddress, &serverLength);
        if (firstServerSocketDescriptor < 0)
            throw SocketAcceptFailedException(listenerSocketDescriptor, false);

        auto secondServerSocket = make_shared<Socket>(new Socket(secondServerSocketDescriptor));
        identifyServerSocketType(secondServerSocket);

        // TODO: Abrir thread para escutar notificações do server

        serverMutex_.unlock();
    }
}

void SocketManager::listenForClientConnections(std::string host, int port)
{
    auto listenerSocketDescriptor = createSocketDescriptor();
    initializeListenerSocket(listenerSocketDescriptor, host, port);

    while (true)
    {
        sockaddr_in clientAddress;
        socklen_t clientLength;

        auto clientSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&clientAddress, &clientLength);
        if (clientSocketDescriptor < 0)
            throw SocketAcceptFailedException(listenerSocketDescriptor);

        auto clientSocket = make_shared<Socket>(new Socket(clientSocketDescriptor));
        thread(SocketManager::identifyClientSocketType, clientSocket).detach();
    }
}

void SocketManager::disconnect()
{

}

// Private functions

int SocketManager::createSocketDescriptor()
{
    auto socketDescriptor = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor == -1)
        throw SocketNotCreatedException();

    return socketDescriptor;
}

void SocketManager::initializeListenerSocket(int socketDescriptor, std::string host, int port)
{
    int option;
    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons((uint16_t)port);
    serverAddress.sin_addr.s_addr = inet_addr(host.c_str());
    bzero(&(serverAddress.sin_zero), 8);

    auto bindResponse = ::bind(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bindResponse < 0)
        throw SocketBindFailedException(socketDescriptor);

    auto listenResponse = ::listen(socketDescriptor, 5);
    if (listenResponse < 0)
        throw SocketListenFailedException(socketDescriptor);
}

void SocketManager::identifyServerSocketType(shared_ptr<Socket> serverSocket)
{
    auto incomingPacket = serverSocket->receive();

    if (incomingPacket->type == PacketType::HELLO_SEND &&
        serverSession_.notificationSocket == nullptr)
    {
        serverSession_.notificationSocket = make_unique<Socket>(serverSocket);
    }
    else if (incomingPacket->type == PacketType::HELLO_RECEIVE &&
        serverSession_.commandSocket == nullptr)
    {
        serverSession_.commandSocket = make_unique<Socket>(serverSocket);
    }
    else
    {
        throw UnexpectedPacketTypeException();
    }
}

void SocketManager::identifyClientSocketType(shared_ptr<Socket> clientSocket)
{
    auto incomingPacket = clientSocket->receive();
    if (incomingPacket->type != PacketType::USERNAME)
        throw UnexpectedPacketTypeException();

    serverMutex_.lock();

    if (serverSession_.commandSocket != nullptr)
    {
        try
        {
            serverSession_.commandSocket->send(*incomingPacket);

            auto sessionAuth = SessionAuth::fromBytes(incomingPacket->_payload);
            addSocketToClientSession(clientSocket, make_unique<SessionAuth>(sessionAuth));
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
        }
    }

    serverMutex_.unlock();

    Packet ackPacket = { PacketType::SERVER_ACK };
    clientSocket->sendIgnoreAck(ackPacket);
}

void SocketManager::addSocketToClientSession(shared_ptr<Socket> clientSocket, unique_ptr<SessionAuth> sessionAuth)
{
    // TODO: Abrir thread para escutar comands se sessão ficar completa
}
