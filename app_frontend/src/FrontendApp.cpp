/**
 * Entry point for frontend application.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "io/CommandLineParser.hpp"
#include "services/SocketManager.hpp"
#include "exceptions/InvalidCommandLineArgsException.hpp"

#include <thread>
#include <iostream>

using namespace std;
using namespace FrontendApp::IO;
using namespace FrontendApp::Service;
using namespace FrontendApp::Exception;

int main(int argc, char** argv)
{
    try
    {
        auto cliArgs = CommandLineParser::parseArgs(argc, argv);
        auto clientConnectionsThread = thread(SocketManager::listenForClientConnections, cliArgs.host, stoi(cliArgs.clientPort));

        SocketManager::listenToServerChanges(cliArgs.host, stoi(cliArgs.serverPort));
        clientConnectionsThread.join();
    }
    catch (const InvalidCommandLineArgsException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
