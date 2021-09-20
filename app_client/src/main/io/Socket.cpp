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
#include "Notification.hpp"
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

    // TODO: How to parse this ack?
    auto ackNotification = receive();
}

void Socket::send(const char* bytes)
{
    auto response = write(socketDescriptor_, bytes, sizeof(bytes));
    if (response < 0)
        throw new SocketWriteFailedException(socketDescriptor_);

    auto ackNotification = receive();
}

Notification Socket::receive()
{
    auto buffer = new char[bufferSize_];
    auto response = read(socketDescriptor_, buffer, bufferSize_);
    if (response < 0)
        throw new SocketReadFailedException(socketDescriptor_);

    auto notification = Notification::parseCsvString(buffer);
    delete buffer;

    return notification;
}
