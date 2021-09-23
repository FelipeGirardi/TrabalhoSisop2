/**
 * Runs app_client application.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "config/GlobalConfig.hpp"
#include "service/SessionManager.hpp"
#include "service/NotificationListener.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "io/CommandLineParser.hpp"
#include "io/Socket.hpp"
#include "Notification.hpp"
#include <thread>
#include <cstring>
#include <string>
#include <thread>

using namespace std;

using ClientApp::Service::SessionManager;
using ClientApp::Service::SessionSockets;
using ClientApp::Service::NotificationListener;
using ClientApp::IO::ConcurrentCommandLine;
using ClientApp::IO::CommandLineParser;
using ClientApp::IO::Socket;

void testHardcodedInteractions(SessionSockets sockets, string profileId);

int main(int argc, char** argv)
{
    ConcurrentCommandLine::initialize();

    auto cliArgs = CommandLineParser::parseClientAppArgs(argc, argv);
    auto sockets = SessionManager::connect(cliArgs.profileId, cliArgs.host, stoi(cliArgs.port));

    auto listenerThread = thread(NotificationListener::listen, sockets.listenerSocket);

    testHardcodedInteractions(sockets, cliArgs.profileId);
    while (true);

    // TODO: Wait for user commands while receiving notifications
    // TODO: Capture scroll up/down when user presses arrow up/down (optional).
    // TODO: Capture exit signals (ctrl+d).

    // Disclaimer: Currently the server has some odd behaviors.
    // - Same messages are sent again when logging in.
    // - Sometimes messages are not received.
    // - I managed to reproduce a concurrency issue by logging in with two users too fast (they started receiving messages from themselves).
}

void testHardcodedInteractions(SessionSockets sockets, string profileId)
{
    // string followName = ConcurrentCommandLine::readLine(50);
    // Packet followPacket = { FOLLOW, (int)followName.length() + 1, 0 };
    // strcat(followPacket._payload, followName.c_str());
    // sockets.senderSocket.send(followPacket);

    Packet sendPacket = { SEND, 14, 0 };
    strcat(sendPacket._payload, string("Hello from " + profileId).c_str());
    sockets.senderSocket.send(sendPacket);
}
