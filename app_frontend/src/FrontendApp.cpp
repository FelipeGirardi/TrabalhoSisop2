/**
 * Entry point for frontend application.
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

int main(int argc, char** argv)
{
    try
    {
        auto cliArgs = CommandLineParser::parseArgs(argc, argv);
    }
    catch (const InvalidCommandLineArgsException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
