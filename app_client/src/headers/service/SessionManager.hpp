/**
 * Thread-safe manager for a single user session.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "io/Socket.hpp"
#include <netdb.h>
#include <mutex>

namespace ClientApp
{
    namespace Service
    {
        struct SessionSockets
        {
            IO::Socket senderSocket;
            IO::Socket listenerSocket;
        };

        class SessionManager
        {
        public:
            static SessionSockets connect(std::string profileId, std::string host, int port);
            static void disconnect();

            static bool isConnected();

        private:
            static IO::Socket createSocket(std::string host);
            static hostent* createHostEntry(std::string host);
            static int createSocketDescriptor();
            static IO::Socket connectSocketToServer(int socketDescriptor, const hostent* hostEntry, int port);
            static void authenticateProfile(std::string profileId);

            static bool isConnected_;
            static SessionSockets* sockets_;
            static std::mutex managerMutex_;
        };
    }
}
