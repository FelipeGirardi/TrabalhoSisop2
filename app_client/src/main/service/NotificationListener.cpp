/**
 * Listener for socket notifications.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "service/NotificationListener.hpp"
#include "service/SessionManager.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "exception/UnexpectedPacketTypeException.hpp"
#include <thread>
#include <chrono>
#include <ctime>
#include <csignal>

using namespace ClientApp::Service;
using namespace ClientApp::Exception;
using namespace notification;
using namespace std::chrono;
using namespace std;

using ClientApp::IO::Socket;
using ClientApp::IO::ConcurrentCommandLine;

// Public functions

void NotificationListener::listen(Socket listenerSocket)
{
    auto incomingPacket = listenerSocket.receive();

    while (incomingPacket->type != EXIT)
    {
        if (incomingPacket->type != NOTIFICATION)
            throw UnexpectedPacketTypeException(NOTIFICATION, incomingPacket->type);

        auto notification = Notification::parseCsvString(incomingPacket->_payload);
        auto printableNotification = formatNotification(notification);
        ConcurrentCommandLine::writeLine(printableNotification);

        incomingPacket = listenerSocket.receive();
    }

    raise(SIGINT);
}

// Private functions

string NotificationListener::formatTime(long secondsSinceEpoch)
{
    auto timePoint = time_point<system_clock>(duration<long>(secondsSinceEpoch));
    auto cTime = system_clock::to_time_t(timePoint);

    char timeBuffer[256];
    strftime(timeBuffer, 256, "%d/%m/%Y %R", localtime(&cTime));

    return string(timeBuffer);
}

string NotificationListener::formatNotification(Notification notification)
{
    auto formattedTime = formatTime(notification.getTime());
    auto profileId = notification.getUsername().empty() ? string("SERVER") : notification.getUsername();

    // [21/09/2021 01:50] 
    // @username: The message
    return string("[") + formattedTime + string("]\n") +
        profileId + string(": ") + notification.getText() + string("\n");
}
