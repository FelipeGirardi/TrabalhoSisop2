/**
 * Definition of a client session.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "Socket.hpp"
#include <string>
#include <memory>

using namespace Common;

namespace FrontendApp
{
    namespace Model
    {
        class ClientSession
        {
        public:
            ClientSession(
                int commandSocketDescriptor,
                int notificationSocketDescriptor,
                std::string clientId,
                std::string sessionUuid);

            std::unique_ptr<const Socket> getCommandSocket();
            std::unique_ptr<const Socket> getNotificationSocket();
            const std::string getClientId();
            const std::string getSessionUuid();

        private:
            const Socket commandSocket_;
            const Socket notificationSocket_;
            const std::string clientId_;
            const std::string sessionUuid_;
        };
    }
}
