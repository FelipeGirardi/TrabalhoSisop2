/**
 * Runs app_client application.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "config/GlobalConfig.hpp"
#include "config/GlobalConstants.hpp"
#include "service/SessionManager.hpp"
#include "service/NotificationListener.hpp"
#include "service/MessageWriter.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "io/CommandLineParser.hpp"
#include "io/Socket.hpp"
#include "exception/InvalidCommandException.hpp"
#include "Notification.hpp"
#include <thread>
#include <cstring>
#include <string>
#include <thread>

using namespace std;

using namespace ClientApp::Exception;
using namespace ClientApp::Service;
using namespace ClientApp::IO;

int main(int argc, char** argv)
{
    ConcurrentCommandLine::initialize();

    auto cliArgs = CommandLineParser::parseClientAppArgs(argc, argv);
    auto sockets = SessionManager::connect(cliArgs.profileId, cliArgs.host, stoi(cliArgs.port));

    auto messageWriter = MessageWriter(string(argv[0]));
    messageWriter.writeLoginMessage(cliArgs.profileId);
    messageWriter.writeHelpMessage();
    messageWriter.writeTimelineHeader();

    auto listenerThread = thread(NotificationListener::listen, sockets.listenerSocket);

    while (true)
    {
        ConcurrentCommandLine::waitInput();
        auto userInput = ConcurrentCommandLine::readLine(GlobalConstant::INPUT_MAX_SIZE);

        try
        {

            auto outboundPacket = CommandLineParser::parseClientAppCommand(userInput);
            sockets.senderSocket.send(outboundPacket);
        }
        catch (const InvalidCommandException& ex)
        {
            messageWriter.writeInvalidCommandMessage(userInput);
        }
    }
}
