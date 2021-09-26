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
#include "Notification.hpp"
#include <cstring>
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

// Static variables

const int Socket::bufferSize_ = BUFFER_SIZE;  // Same as Packet

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
        throw new SocketWriteFailedException(socketDescriptor_);

    auto ackNotification = receive();
    if (ackNotification.getID() == to_string(SERVER_ERROR))
        throw new ServerNotAcknowledgedException(socketDescriptor_);
}

void Socket::send(const char* bytes)
{
    auto response = write(socketDescriptor_, bytes, sizeof(bytes));
    if (response < 0)
        throw new SocketWriteFailedException(socketDescriptor_);

    auto ackNotification = receive();
    if (ackNotification.getID() == to_string(SERVER_ERROR))
        throw new ServerNotAcknowledgedException(socketDescriptor_);
}

Notification Socket::receive()
{
    Packet incomingPacket;

    auto response = read(socketDescriptor_, &incomingPacket, sizeof(Packet));
    if (response < 0)
        throw new SocketReadFailedException(socketDescriptor_);

    if (incomingPacket.type == NOTIFICATION)
        return Notification::parseCsvString(incomingPacket._payload);

    return Notification(to_string(incomingPacket.type), incomingPacket._payload, "./serverApp", 0, 0);
}
