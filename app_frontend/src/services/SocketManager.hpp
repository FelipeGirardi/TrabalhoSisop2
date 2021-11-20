/**
 * Manage sockets.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "SessionAuth.hpp"
#include "io/Socket.hpp"

#include <list>
#include <mutex>
#include <string>
#include <memory>
#include <unordered_map>

namespace FrontendApp
{
    namespace Service
    {
        struct ServerSession
        {
            IO::Socket* commandSocket;
            IO::Socket* notificationSocket;
        };

        struct ClientSession : ServerSession
        {
            std::string uuid;
            std::string profileId;
        };

        class SocketManager
        {
        public:
            static void listenToServerChanges(std::string host, int port);
            static void listenForClientConnections(std::string host, int port);
            static void listenForServerNotifications();
            static void listenForClientCommands(ClientSession clientSession);
            static void disconnectAllClients();

        private:
            static int createSocketDescriptor();
            static void initializeListenerSocket(int socketDescriptor, std::string host, int port);
            static void identifyServerSocketType(IO::Socket* serverSocket);
            static void identifyClientSocketType(IO::Socket* clientSocket);
            static void addSocketToClientSession(IO::Socket* clientSocket, Common::SessionAuth* sessionAuth);

            static std::mutex serverMutex_;
            static ServerSession serverSession_;
            static std::unordered_map<std::string, std::list<ClientSession>> clientSessions_;
        };
    }
}
