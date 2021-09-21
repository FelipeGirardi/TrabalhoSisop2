/**
 * Handler for uncaught exceptions in runtime.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "config/GlobalExceptionHandler.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include <exception>

using namespace ClientApp::Config;
using namespace std;

using ClientApp::IO::ConcurrentCommandLine;

// Public methods

void GlobalExceptionHandler::enable()
{
    set_terminate(handleUncaughtException);
}

void GlobalExceptionHandler::handleUncaughtException()
{
    ConcurrentCommandLine::terminate();
}

