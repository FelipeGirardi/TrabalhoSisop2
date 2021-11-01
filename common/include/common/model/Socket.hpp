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

namespace Common
{
    namespace Model
    {
        class Socket
        {
        public:
            Socket(int socketDescriptor);

            int getDescriptor();

            /**
             * Sends packet through this socket.
             *
             * \param packet Packet to send.
             * \throws SocketWriteFailedException when any failure occurs.
             */
            void send(const Packet& packet);

            /**
             * Receives packet through this socket.
             *
             * \return Smart pointer to received packet.
             * \throws SocketReadFailedException when any failure occurs.
             */
            std::unique_ptr<Packet> receive();

        private:
            const int socketDescriptor_;
        };
    }
}

