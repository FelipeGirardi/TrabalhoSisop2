/**
 * Interacts with connected socket.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "Notification.hpp"
#include "Packet.hpp"
#include <string>

namespace ClientApp
{
    namespace IO
    {
        class Socket
        {
        public:
            Socket(int socketDescriptor);

            int getDescriptor();

            void send(Packet packet);
            void send(const char* bytes);
            notification::Notification receive();

        private:
            static const int bufferSize_;
            const int socketDescriptor_;
        };

    }
}
