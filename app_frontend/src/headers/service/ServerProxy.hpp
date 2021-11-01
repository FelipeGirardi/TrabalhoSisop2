/**
 * Remote proxy for interactions with the server.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "common/model/Socket.hpp"
#include "Packet.hpp"
#include <memory>
#include <string>
#include <mutex>

namespace FrontendApp
{
    namespace Service
    {
        struct ServerUrl
        {
            const std::string host;
            const int port;
        };

        class ServerProxy
        {
        public:
            static ServerProxy connect();
            static void listenPrimaryServerChanges();
            static ServerUrl onPrimaryServerChange();

            void send(const Packet& packet);
            std::unique_ptr<Packet> listen();
            void disconnect();

        private:
            ServerProxy(Common::Model::Socket senderSocket, Common::Model::Socket listenerSocket);

            static std::unique_ptr<Common::Model::Socket> primaryServerSocket_;
            static ServerUrl primaryServerUrl_;
            static bool hasNewPrimaryServer_;
            static std::mutex primaryServerMutex_;

            bool isConnected_;
            std::unique_ptr<Common::Model::Socket> senderSocket_;
            std::unique_ptr<Common::Model::Socket> listenerSocket_;

            static bool hasPrimaryServerChanged();

            bool isConnected();
        };
    }
}
