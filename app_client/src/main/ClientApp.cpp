/**
 * Runs app_client application.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "io/ConcurrentCommandLine.hpp"
#include "io/CommandLineParser.hpp"
#include "io/SessionManager.hpp"
#include "io/Socket.hpp"
#include "Notification.hpp"
#include <exception>
#include <iostream>
#include <thread>
#include <string>

using namespace std;

using ClientApp::IO::ConcurrentCommandLine;
using ClientApp::IO::CommandLineParser;
using ClientApp::IO::SessionManager;
using ClientApp::IO::Socket;

void listenForNotifications(Socket notificationSocket);
void globalExceptionHandler();

int main(int argc, char** argv)
{
    ConcurrentCommandLine::initialize();
    set_terminate(globalExceptionHandler);

    auto cliArgs = CommandLineParser::parseClientAppArgs(argc, argv);
    auto sockets = SessionManager::connect(cliArgs.profileId, cliArgs.host, stoi(cliArgs.port));

    auto notificationsThread = thread(listenForNotifications, sockets.listenerSocket);

    while (true);

    // TODO: Wait for user commands while receiving notifications
    // TODO: Capture exit signals (ctrl+c, ctrl+d).
    // TODO: Disconnect sockets.
    ConcurrentCommandLine::terminate();
}

void listenForNotifications(Socket notificationSocket)
{
    while (true)
    {
        auto notification = notificationSocket.receive();

        auto printableNotification =
            notification.getUsername() + " " + to_string(notification.getTime()) + ": " +
            notification.getText() + "\r\n";
        ConcurrentCommandLine::writeLine(printableNotification);
    }
}

void globalExceptionHandler()
{
    ConcurrentCommandLine::terminate();
}
