/**
 * Interacts with connected socket.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "Packet.hpp"

#include <string>
#include <memory>

namespace FrontendApp
{
    namespace IO
    {
        class Socket
        {
        public:
            Socket(int socketDescriptor);
            ~Socket();

            int getDescriptor();

            void send(Packet packet);
            void sendIgnoreAck(Packet packet);
            std::unique_ptr<Packet> receive();

        private:
            const int socketDescriptor_;
        };
    }
}
