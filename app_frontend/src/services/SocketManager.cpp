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
#include <cstring>
#include <memory>
#include <iostream>
#include <exception>
#include <errno.h>
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
    cout << "Started to listen for primary server changes at " << host << ":" << port << endl;
    setbuf(stdout, nullptr);
    auto listenerSocketDescriptor = createSocketDescriptor();
    initializeListenerSocket(listenerSocketDescriptor, host, port);

    while (true)
    {
        sockaddr_in serverAddress;
        socklen_t serverLength = sizeof(serverAddress);

        cout << "Waiting for primary server changes...";
        auto firstServerSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&serverAddress, &serverLength);
        if (firstServerSocketDescriptor < 0)
        {
            cout << "Failed to accept first server socket" << endl;
            cout << "Failure reason: " << strerror(errno) << endl;
            ::close(listenerSocketDescriptor);
            throw SocketAcceptFailedException(listenerSocketDescriptor, false);
        }

        serverMutex_.lock();

        serverSession_.commandSocket = nullptr;
        serverSession_.notificationSocket = nullptr;

        auto firstServerSocket = new Socket(firstServerSocketDescriptor);
        identifyServerSocketType(firstServerSocket);

        auto secondServerSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&serverAddress, &serverLength);
        if (secondServerSocketDescriptor < 0)
        {
            cout << "Failed to accept second server socket" << endl;
            cout << "Failure reason: " << strerror(errno) << endl;
            ::close(listenerSocketDescriptor);
            throw SocketAcceptFailedException(listenerSocketDescriptor, false);
        }

        auto secondServerSocket = new Socket(secondServerSocketDescriptor);
        identifyServerSocketType(secondServerSocket);

        thread(listenForServerNotifications).detach();

        serverMutex_.unlock();
    }
}

void SocketManager::listenForClientConnections(string host, int port)
{
    cout << "Started to listen for client connections at " << host << ":" << port << endl;
    auto listenerSocketDescriptor = createSocketDescriptor();
    initializeListenerSocket(listenerSocketDescriptor, host, port);

    while (true)
    {
        sockaddr_in clientAddress;
        socklen_t clientLength;

        cout << "Waiting for client connections..." << endl;
        auto clientSocketDescriptor = ::accept(listenerSocketDescriptor, (sockaddr*)&clientAddress, &clientLength);
        if (clientSocketDescriptor < 0)
        {
            cout << "Failed to accept client connection" << endl;
            cout << "Failure reason: " << strerror(errno) << endl;
            ::close(listenerSocketDescriptor);
            throw SocketAcceptFailedException(listenerSocketDescriptor);
        }
        cout << "Received new client connection" << endl;

        auto clientSocket = new Socket(clientSocketDescriptor);
        thread(SocketManager::identifyClientSocketType, clientSocket).detach();
    }
}

void SocketManager::listenForServerNotifications()
{
    Packet* incomingPacket = nullptr;
    try
    {
        incomingPacket = serverSession_.notificationSocket->receive();
    }
    catch (...)
    {
        cout << "Lost connection to primary server socket..." << endl;
        return;
    }

    while (incomingPacket->type != EXIT)
    {
        if (incomingPacket->type != NOTIFICATION)
        {
            cout << "Lost connection to primary server socket..." << endl;
            return;
        }
        cout << "Received notification from server";

        try
        {
            auto frontendPayload = FrontEndPayload::fromBytes(incomingPacket->_payload);
            auto sessionsCount = 0;
            if (clientSessions_.count(frontendPayload->senderUsername) > 0)
            {
                auto activeSessions = clientSessions_[frontendPayload->senderUsername];
                for (auto& session : activeSessions)
                {
                    session.notificationSocket->sendIgnoreAck(*incomingPacket);
                    sessionsCount++;
                }
            }
            cout << "Sent notification to " << sessionsCount << "client sessions from " << frontendPayload->senderUsername << endl;

            Packet ackPacket = { PacketType::SERVER_ACK };
            serverSession_.notificationSocket->sendIgnoreAck(ackPacket);
            cout << "Sent ACK to server" << endl;
        }
        catch (...)
        {
            cout << "Could not send notification to client sessions: sending NACK to server" << endl;
            Packet nackPacket = { PacketType::SERVER_ERROR };
            serverSession_.notificationSocket->sendIgnoreAck(nackPacket);
        }

        try
        {
            incomingPacket = serverSession_.notificationSocket->receive();
        }
        catch (...)
        {
            cout << "Lost connection to primary server socket..." << endl;
            return;
        }
    }

    serverSession_.commandSocket = nullptr;
    serverSession_.notificationSocket = nullptr;
}

void SocketManager::listenForClientCommands(ClientSession clientSession)
{
    cout << "Waiting for commands from client " << clientSession.profileId << " - " << clientSession.uuid << endl;
    auto incomingPacket = clientSession.commandSocket->receive();
    cout << "Received command from client " << clientSession.profileId << " - " << clientSession.uuid << endl;

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
            cout << "Sent command to server: sending ACK to client " << clientSession.profileId << " - " << clientSession.uuid << endl;
        }
        catch (...)
        {
            cout << "Could not send command to server: sending NACK to client" << clientSession.profileId << " - " << clientSession.uuid << endl;
            Packet nackPacket = { PacketType::SERVER_ERROR };
            clientSession.commandSocket->sendIgnoreAck(nackPacket);
        }

        serverMutex_.unlock();

        cout << "Waiting for commands from client " << clientSession.profileId << " - " << clientSession.uuid << endl;
        incomingPacket = clientSession.commandSocket->receive();
    }

    cout << "EXIT received from client " << clientSession.profileId << " - " << clientSession.uuid << endl;

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
            break;
        }
    }

    activeSessions.erase(deleteIterator);
}

void SocketManager::closeServerSockets()
{
    ::close(serverSession_.commandSocket->getDescriptor());
    ::close(serverSession_.notificationSocket->getDescriptor());
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
            ::close(session.commandSocket->getDescriptor());
            ::close(session.notificationSocket->getDescriptor());
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
    int option = 1;
    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
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
        cout << "Primary server changed: received notification socket" << endl;
        serverSession_.notificationSocket = serverSocket;
    }
    else if (incomingPacket->type == PacketType::HELLO_RECEIVE &&
        serverSession_.commandSocket == nullptr)
    {
        cout << "Primary server changed: received command socket" << endl;
        serverSession_.commandSocket = serverSocket;
    }
    else
    {
        Packet nackPacket = { PacketType::SERVER_ERROR };
        serverSocket->sendIgnoreAck(nackPacket);

        cout << "ERROR: Primary server changed: received invalid socket" << endl;
        incomingPacket->printItself();
        throw UnexpectedPacketTypeException();
    }

    Packet ackPacket = { PacketType::SERVER_ACK };
    serverSocket->sendIgnoreAck(ackPacket);
}

void SocketManager::identifyClientSocketType(Socket* clientSocket)
{
    cout << "Identifying client connection socket type..." << endl;
    auto incomingPacket = clientSocket->receive();
    if (incomingPacket->type != PacketType::USERNAME)
        throw UnexpectedPacketTypeException();

    serverMutex_.lock();

    try
    {
        auto sessionAuth = SessionAuth::fromBytes(incomingPacket->_payload);
        addSocketToClientSession(clientSocket, sessionAuth);
    }
    catch (const exception& e)
    {
        cout << "Could not identify client connection socket type: sending NACK to client" << endl;

        Packet nackPacket = { PacketType::SERVER_ERROR };
        clientSocket->sendIgnoreAck(nackPacket);
    }

    serverMutex_.unlock();
}

void SocketManager::addSocketToClientSession(Socket* clientSocket, SessionAuth* sessionAuth)
{
    if (clientSessions_.count(sessionAuth->getProfileId()) == 0)
    {
        cout << "First connection from client " << sessionAuth->getProfileId() << endl;
        list<ClientSession> sessions;
        clientSessions_.insert(pair(sessionAuth->getProfileId(), sessions));
    }

    auto hasSession = false;
    auto& activeSessions = clientSessions_[sessionAuth->getProfileId()];
    for (auto& session : activeSessions)
    {
        if (session.uuid == sessionAuth->getUuid())
        {
            cout << "Session " << sessionAuth->getUuid() << " found for client " << sessionAuth->getProfileId() << endl;
            hasSession = true;
            if (sessionAuth->getSocketType() == SocketType::COMMAND_SOCKET)
                session.commandSocket = clientSocket;
            else if (sessionAuth->getSocketType() == SocketType::NOTIFICATION_SOCKET)
                session.notificationSocket = clientSocket;

            if (session.commandSocket != nullptr && session.notificationSocket != nullptr)
            {
                cout << "All sockets set for " << sessionAuth->getProfileId() << " - " << sessionAuth->getUuid() << endl;
                FrontEndPayload authPayload;
                strncpy(authPayload.senderUsername, sessionAuth->getProfileId().c_str(), 100);
                strncpy(authPayload.commandContent, sessionAuth->getUuid().c_str(), 128);

                Packet loginPacket = { PacketType::LOGIN, 0, time(NULL) };
                memcpy(loginPacket._payload, authPayload.toBytes(), sizeof(FrontEndPayload));

                serverSession_.commandSocket->send(loginPacket);

                cout << "Creating thread to listen for commands for " << sessionAuth->getProfileId() << endl;
                thread(listenForClientCommands, session).detach();
            }

            break;
        }
    }

    if (!hasSession)
    {
        cout << "Creating session for " << sessionAuth->getProfileId() << endl;
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

    Packet ackPacket = { PacketType::SERVER_ACK };
    clientSocket->sendIgnoreAck(ackPacket);
}
