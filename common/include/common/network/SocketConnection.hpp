/**
 * Functions to support connecting sockets.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "common/exception/SocketConnectionDeniedException.hpp"
#include "common/exception/SocketNotCreatedException.hpp"
#include "common/exception/HostNotFoundException.hpp"
#include "common/model/Socket.hpp"
#include <netdb.h>

namespace Common
{
    namespace Network
    {
        /**
         * Creates hostent instance.
         *
         * \param host      The host to connect to.
         * \return Instance of hostent.
         *
         * \throws HostNotFoundException when host cannot be found.
         */
        hostent* createHostEntry(std::string host);

        /**
         * Creates socket descriptor with the following properties:
         *   - Domain: IF_NET.
         *   - Type: SOCK_STREAM.
         *   - Protocol: 0 (chosen automatically).
         *
         * \return Created socket descriptor.
         *
         * \throws SocketNotCreatedException when socket cannot be created.
         */
        int createSocketDescriptor();

        /**
         * Connects socket descritor to host.
         *
         * \param socketDescriptor  The socket to connect.
         * \param hostEntry         The host to connect to.
         * \param port              The port to connect to.
         * \return Connected socket.
         *
         * \throws SocketConnectionDeniedException when host denies connection.
         */
        Model::Socket connectSocketToHost(int socketDescriptor, const hostent* hostEntry, int port);

        /**
         * Closes socket.
         *
         * \param socketDescriptor  The socket to close.
         */
        void closeSocket(int socketDescriptor);
    }
}


