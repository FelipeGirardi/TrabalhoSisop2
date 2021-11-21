/**
 * Parses command line arguments.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "io/CommandLineParser.hpp"
#include "exceptions/InvalidCommandLineArgsException.hpp"

#include <iostream>

using namespace std;
using namespace FrontendApp::IO;
using namespace FrontendApp::Exception;


// Static variables

const unsigned CommandLineParser::hostIndex_ = 1U;
const unsigned CommandLineParser::serverPortIndex_ = 2U;
const unsigned CommandLineParser::clientPortIndex_ = 3U;


// Public functions

FrontendArgs CommandLineParser::parseArgs(int argc, char** argv)
{
    if ((argc < getArgsMinSize()) || (argv == nullptr))
        throw InvalidCommandLineArgsException();

    FrontendArgs parsedArgs = {
        argv[hostIndex_],
        argv[clientPortIndex_],
        argv[serverPortIndex_]
    };

    return parsedArgs;
}


// Private functions

unsigned CommandLineParser::getArgsMinSize()
{
    return serverPortIndex_ + 1;
}
