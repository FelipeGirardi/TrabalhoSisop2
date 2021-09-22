/**
 * Parses command line arguments.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "io/CommandLineParser.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "exception/InvalidArgsException.hpp"
#include <exception>

using namespace ClientApp::IO;
using ClientApp::Exception::InvalidArgsException;

// Public methods

ClientAppArgs CommandLineParser::parseClientAppArgs(int argc, char** argv)
{
    if ((argc <= PORT_INDEX) || (argv == nullptr))
    {
        ConcurrentCommandLine::writeLine("usage: ./app_client [profile] [server-host] [server-port]");
        throw new InvalidArgsException();
    }

    ClientAppArgs parsedArgs = {
        argv[PROFILE_ID_INDEX],
        argv[HOST_INDEX],
        argv[PORT_INDEX]
    };

    return parsedArgs;
}
