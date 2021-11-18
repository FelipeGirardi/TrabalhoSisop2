/**
 * Handler for signals raised in runtime.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "config/GlobalSignalHandler.hpp"
#include "service/SessionManager.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include <csignal>
#include <cstdlib>

using namespace ClientApp::Config;

using ClientApp::Service::SessionManager;
using ClientApp::IO::ConcurrentCommandLine;

bool GlobalSignalHandler::enable()
{
    signal(SIGINT, handleInteractiveAttentionSignal);
    // signal(SIGABRT, handleAbortSignal);
    return true;
}

void GlobalSignalHandler::handleInteractiveAttentionSignal(int signalNum)
{
    if (signalNum == SIGINT)
    {
        SessionManager::disconnect();
        ConcurrentCommandLine::terminate();
    }

    exit(signalNum);
}

void GlobalSignalHandler::handleAbortSignal(int signalNum)
{
    if (signalNum == SIGABRT)
    {
        ConcurrentCommandLine::terminate();
    }

    exit(signalNum);
}
