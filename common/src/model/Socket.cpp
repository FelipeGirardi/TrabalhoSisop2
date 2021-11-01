/**
 * Interacts with connected socket.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "exception/SocketWriteFailedException.hpp"
#include "exception/SocketReadFailedException.hpp"
#include "Notification.hpp"
#include "Socket.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <memory>

using namespace Common;
using namespace Common::Exception;
using namespace std;


// Public methods

Socket::Socket(int socketDescriptor)
    : socketDescriptor_(socketDescriptor) { }

int Socket::getDescriptor()
{
    return socketDescriptor_;
}

void Socket::send(const Packet& packet)
{
    auto response = write(socketDescriptor_, &packet, sizeof(packet));
    if (response < 0)
        throw SocketWriteFailedException(socketDescriptor_);
}

unique_ptr<Packet> Socket::receive()
{
    auto incomingPacket = unique_ptr<Packet>(new Packet());

    auto response = read(socketDescriptor_, incomingPacket.get(), sizeof(Packet));
    if (response < 0)
        throw SocketReadFailedException(socketDescriptor_);

    return incomingPacket;
}
