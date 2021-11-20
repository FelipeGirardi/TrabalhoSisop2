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
            std::unique_ptr<IO::Socket> commandSocket;
            std::unique_ptr<IO::Socket> notificationSocket;
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

            // TODO: Nos listeners, abrir thread do primary server e dos clientes
            // TODO: Receber coisas/notificaçoes do servidor + enviar para o cliente
            // TODO: Receber comandos do cliente + enviar para o servidor
            // TODO: Encerrar sessões em comandos de EXIT
            // TODO: Na main, abrir threads para primary server e client connections

            static void disconnect();

        private:
            static int createSocketDescriptor();
            static void initializeListenerSocket(int socketDescriptor, std::string host, int port);
            static void identifyServerSocketType(std::shared_ptr<IO::Socket> serverSocket);
            static void identifyClientSocketType(std::shared_ptr<IO::Socket> clientSocket);
            static void addSocketToClientSession(std::shared_ptr<IO::Socket> clientSocket, std::unique_ptr<Common::SessionAuth> sessionAuth);

            static std::mutex serverMutex_;
            static ServerSession serverSession_;
            static std::unordered_map<std::string, std::list<ClientSession>> clientSessions_;
        };
    }
}
