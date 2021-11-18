/**
 * Concurrent listener for socket notifications.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "io/Socket.hpp"
#include "Notification.hpp"
#include <string>

namespace ClientApp
{
    namespace Service
    {
        class NotificationListener
        {
        public:
            static void listen(IO::Socket listenerSocket);

        private:
            static std::string formatTime(long secondsSinceEpoch);
            static std::string formatNotification(notification::Notification notification);
        };
    }
}
