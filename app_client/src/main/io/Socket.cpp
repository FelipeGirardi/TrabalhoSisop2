/**
 * Manages socket connection.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "io/Socket.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "exception/SocketReadFailedException.hpp"
#include "exception/SocketWriteFailedException.hpp"
#include "exception/ServerNotAcknowledgedException.hpp"
#include "exception/UnexpectedPacketTypeException.hpp"
#include "Notification.hpp"
#include <cstring>
#include <memory>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace ClientApp::IO;
using namespace std;

using notification::Notification;
using ClientApp::Exception::SocketReadFailedException;
using ClientApp::Exception::SocketWriteFailedException;
using ClientApp::Exception::ServerNotAcknowledgedException;
using ClientApp::Exception::UnexpectedPacketTypeException;

// Public methods

Socket::Socket(int socketDescriptor)
    : socketDescriptor_(socketDescriptor) { }

int Socket::getDescriptor()
{
    return socketDescriptor_;
}

void Socket::send(Packet packet)
{
    auto response = write(socketDescriptor_, &packet, sizeof(packet));
    if (response < 0)
        throw SocketWriteFailedException(socketDescriptor_);

    auto ackPacket = receivePacket();
    if (ackPacket->type != SERVER_ACK)
        throw ServerNotAcknowledgedException(socketDescriptor_);
}

void Socket::send(const char* bytes)
{
    auto response = write(socketDescriptor_, bytes, sizeof(bytes));
    if (response < 0)
        throw SocketWriteFailedException(socketDescriptor_);

    auto ackPacket = receivePacket();
    if (ackPacket->type != SERVER_ACK)
        throw ServerNotAcknowledgedException(socketDescriptor_);
}

Notification Socket::receive()
{
    auto incomingPacket = receivePacket();

    if (incomingPacket->type != NOTIFICATION)
        throw UnexpectedPacketTypeException(NOTIFICATION, incomingPacket->type);

    return Notification::parseCsvString(incomingPacket->_payload);
}

// Private methods

std::unique_ptr<Packet> Socket::receivePacket()
{
    auto incomingPacket = unique_ptr<Packet>(new Packet());

    auto response = read(socketDescriptor_, incomingPacket.get(), sizeof(Packet));
    if (response < 0)
        throw SocketReadFailedException(socketDescriptor_);

    return incomingPacket;
}
