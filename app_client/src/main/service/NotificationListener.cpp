/**
 * Listener for socket notifications.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "service/NotificationListener.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include <thread>
#include <chrono>
#include <ctime>

using namespace ClientApp::Service;
using namespace notification;
using namespace std::chrono;
using namespace std;

using ClientApp::IO::Socket;
using ClientApp::IO::ConcurrentCommandLine;

// Public functions

void NotificationListener::listen(Socket listenerSocket)
{
    while (true)
    {
        auto notification = listenerSocket.receive();

        // TODO: Identify why server sends trash in messages
        if (!notification.getUsername().empty())
        {
            auto printableNotification = formatNotification(notification);
            ConcurrentCommandLine::writeLine(printableNotification);
        }
    }
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
    return "[" + formattedTime + "]\n" + profileId + ": " + notification.getText() + "\n";
}
