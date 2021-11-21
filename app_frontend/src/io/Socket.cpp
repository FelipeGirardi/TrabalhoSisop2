/**
 * Manages socket connection.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "Notification.hpp"
#include "io/Socket.hpp"
#include "exceptions/SocketReadFailedException.hpp"
#include "exceptions/SocketWriteFailedException.hpp"
#include "exceptions/ServerNotAcknowledgedException.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
using namespace notification;
using namespace FrontendApp::IO;
using namespace FrontendApp::Exception;


// Public funtions

Socket::Socket(int socketDescriptor)
    : socketDescriptor_(socketDescriptor) { }

Socket::~Socket()
{
    ::close(getDescriptor());
}

int Socket::getDescriptor()
{
    return socketDescriptor_;
}

void Socket::send(Packet packet)
{
    auto response = ::write(socketDescriptor_, &packet, sizeof(Packet));
    if (response < 0)
        throw SocketWriteFailedException(socketDescriptor_);

    auto ackPacket = receive();
    if (ackPacket->type != SERVER_ACK)
        throw ServerNotAcknowledgedException(socketDescriptor_);
}

void Socket::sendIgnoreAck(Packet packet)
{
    auto response = ::write(socketDescriptor_, &packet, sizeof(Packet));
    if (response < 0)
        throw SocketWriteFailedException(socketDescriptor_);
}

Packet* Socket::receive()
{
    auto incomingPacket = new Packet();

    auto response = ::read(socketDescriptor_, incomingPacket, sizeof(Packet));
    if (response < 0)
        throw SocketReadFailedException(socketDescriptor_);

    return incomingPacket;
}
