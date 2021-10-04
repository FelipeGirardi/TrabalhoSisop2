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
#include <memory>

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
            std::unique_ptr<Packet> receivePacket();

            const int socketDescriptor_;
        };

    }
}
