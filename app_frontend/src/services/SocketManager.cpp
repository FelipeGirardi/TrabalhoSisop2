/**
 * Manage sockets.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "Notification.hpp"
#include "FrontEndPayload.hpp"
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
using namespace notification;
using namespace FrontendApp::IO;
using namespace FrontendApp::Service;
using namespace FrontendApp::Exception;

// Static variables

mutex SocketManager::serverMutex_;
ServerSession SocketManager::serverSession_ = { nullptr, nullptr };
std::unordered_map<std::string, std::list<ClientSession>> SocketManager::clientSessions_;


// Public methods

void SocketManager::listenToServerChanges(string host, int port)
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

        delete serverSession_.commandSocket;
        delete serverSession_.notificationSocket;
        serverSession_.commandSocket = nullptr;
        serverSession_.notificationSocket = nullptr;

        auto firstServerSocket = new Socket(firstServerSocketDescriptor);
        identifyServerSocketType(firstServerSocket);

        auto secondServerSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&serverAddress, &serverLength);
        if (firstServerSocketDescriptor < 0)
            throw SocketAcceptFailedException(listenerSocketDescriptor, false);

        auto secondServerSocket = new Socket(secondServerSocketDescriptor);
        identifyServerSocketType(secondServerSocket);

        thread(listenForServerNotifications).detach();

        serverMutex_.unlock();
    }
}

void SocketManager::listenForClientConnections(string host, int port)
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

        auto clientSocket = new Socket(clientSocketDescriptor);
        thread(SocketManager::identifyClientSocketType, clientSocket).detach();
    }
}

void SocketManager::listenForServerNotifications()
{
    auto incomingPacket = serverSession_.notificationSocket->receive();

    while (incomingPacket->type != EXIT)
    {
        if (incomingPacket->type != NOTIFICATION)
        {
            cerr << "SocketManager: lost connection to server socket..." << endl;
            throw UnexpectedPacketTypeException();
        }

        try
        {
            auto frontendPayload = FrontEndPayload::fromBytes(incomingPacket->_payload);
            if (clientSessions_.count(frontendPayload->senderUsername) > 0)
            {
                auto activeSessions = clientSessions_[frontendPayload->senderUsername];
                for (auto& session : activeSessions)
                    session.notificationSocket->sendIgnoreAck(*incomingPacket);
            }

            Packet ackPacket = { PacketType::SERVER_ACK };
            serverSession_.notificationSocket->sendIgnoreAck(ackPacket);
        }
        catch (...)
        {
            Packet nackPacket = { PacketType::SERVER_ERROR };
            serverSession_.notificationSocket->sendIgnoreAck(nackPacket);
        }

        incomingPacket = serverSession_.notificationSocket->receive();
    }

    disconnectAllClients();

    serverMutex_.lock();
    delete serverSession_.commandSocket;
    delete serverSession_.notificationSocket;
    serverSession_.commandSocket = nullptr;
    serverSession_.notificationSocket = nullptr;
}

void SocketManager::listenForClientCommands(ClientSession clientSession)
{
    auto incomingPacket = clientSession.notificationSocket->receive();

    while (incomingPacket->type != EXIT)
    {
        FrontEndPayload frontendPayload;
        strcpy(frontendPayload.senderUsername, clientSession.profileId.c_str());
        strcpy(frontendPayload.commandContent, incomingPacket->_payload);

        memcpy(incomingPacket->_payload, frontendPayload.toBytes(), sizeof(FrontEndPayload));

        serverMutex_.lock();

        try
        {
            serverSession_.commandSocket->send(*incomingPacket);

            Packet ackPacket = { PacketType::SERVER_ACK };
            clientSession.commandSocket->sendIgnoreAck(ackPacket);
        }
        catch (...)
        {
            Packet nackPacket = { PacketType::SERVER_ERROR };
            clientSession.commandSocket->sendIgnoreAck(nackPacket);
        }

        serverMutex_.unlock();
    }

    FrontEndPayload exitPayload;
    strcpy(exitPayload.senderUsername, clientSession.profileId.c_str());
    strcpy(exitPayload.commandContent, clientSession.uuid.c_str());

    memcpy(incomingPacket->_payload, exitPayload.toBytes(), sizeof(FrontEndPayload));

    serverMutex_.lock();

    try
    {
        serverSession_.commandSocket->send(*incomingPacket);

        Packet ackPacket = { PacketType::SERVER_ACK };
        clientSession.commandSocket->sendIgnoreAck(ackPacket);
    }
    catch (...)
    {
        Packet nackPacket = { PacketType::SERVER_ERROR };
        clientSession.commandSocket->sendIgnoreAck(nackPacket);
    }

    serverMutex_.unlock();

    auto& activeSessions = clientSessions_[clientSession.profileId];
    auto deleteIterator = activeSessions.begin();
    for (auto iterator = activeSessions.begin(); iterator != activeSessions.end(); iterator++)
    {
        if (iterator->uuid == clientSession.uuid)
        {
            deleteIterator = iterator;
            delete iterator->commandSocket;
            delete iterator->notificationSocket;
            break;
        }
    }

    activeSessions.erase(deleteIterator);
}

void SocketManager::disconnectAllClients()
{
    Packet exitPacket = { PacketType::EXIT };

    for (auto& client : clientSessions_)
    {
        auto activeSessions = client.second;
        for (auto& session : activeSessions)
        {
            session.notificationSocket->sendIgnoreAck(exitPacket);
            delete session.notificationSocket;
            delete session.commandSocket;
        }
    }

    clientSessions_.clear();
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

void SocketManager::identifyServerSocketType(Socket* serverSocket)
{
    auto incomingPacket = serverSocket->receive();

    if (incomingPacket->type == PacketType::HELLO_SEND &&
        serverSession_.notificationSocket == nullptr)
    {
        serverSession_.notificationSocket = serverSocket;
    }
    else if (incomingPacket->type == PacketType::HELLO_RECEIVE &&
        serverSession_.commandSocket == nullptr)
    {
        serverSession_.commandSocket = serverSocket;
    }
    else
    {
        throw UnexpectedPacketTypeException();
    }
}

void SocketManager::identifyClientSocketType(Socket* clientSocket)
{
    auto incomingPacket = clientSocket->receive();
    if (incomingPacket->type != PacketType::USERNAME)
        throw UnexpectedPacketTypeException();

    serverMutex_.lock();

    try
    {
        serverSession_.commandSocket->send(*incomingPacket);

        auto sessionAuth = SessionAuth::fromBytes(incomingPacket->_payload);
        addSocketToClientSession(clientSocket, sessionAuth);
        delete sessionAuth;

        Packet ackPacket = { PacketType::SERVER_ACK };
        clientSocket->sendIgnoreAck(ackPacket);
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;

        Packet nackPacket = { PacketType::SERVER_ERROR };
        clientSocket->sendIgnoreAck(nackPacket);
        delete clientSocket;
    }

    serverMutex_.unlock();
}

void SocketManager::addSocketToClientSession(Socket* clientSocket, SessionAuth* sessionAuth)
{
    if (clientSessions_.count(sessionAuth->getProfileId()) == 0)
    {
        list<ClientSession> sessions;
        clientSessions_.insert(pair(sessionAuth->getProfileId(), sessions));
    }

    auto hasSession = false;
    auto& activeSessions = clientSessions_[sessionAuth->getProfileId()];
    for (auto& session : activeSessions)
    {
        if (session.uuid == sessionAuth->getUuid())
        {
            hasSession = true;
            if (sessionAuth->getSocketType() == SocketType::COMMAND_SOCKET)
                session.commandSocket = clientSocket;
            else if (sessionAuth->getSocketType() == SocketType::NOTIFICATION_SOCKET)
                session.notificationSocket = clientSocket;

            if (session.commandSocket != nullptr && session.notificationSocket != nullptr)
                thread(listenForClientCommands, session).detach();

            break;
        }
    }

    if (!hasSession)
    {
        if (sessionAuth->getSocketType() == SocketType::COMMAND_SOCKET)
        {
            ClientSession newSession = { clientSocket, nullptr, sessionAuth->getUuid(), sessionAuth->getProfileId() };
            activeSessions.push_front(newSession);
        }
        else
        {
            ClientSession newSession = { nullptr, clientSocket, sessionAuth->getUuid(), sessionAuth->getProfileId() };
            activeSessions.push_front(newSession);
        }
    }
}
