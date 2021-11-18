/**
 * Writes common alert messages.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "service/MessageWriter.hpp"
#include "io/CommandLineParser.hpp"
#include "io/ConcurrentCommandLine.hpp"

using namespace ClientApp::Service;
using namespace ClientApp::IO;
using namespace std;


MessageWriter::MessageWriter(std::string appName)
    : appName_(appName)
{
    // Empty body
}

void MessageWriter::writeHelpMessage()
{
    auto helpMessage =
        string("usage: <command> <args>\n\n") +
        string("These are the available commands:\n") +
        string("    send <message>\t\tSends a message to all followers (up to 128 characters)\n") +
        string("    follow <profile>\t\tFollows a profile (4 to 20 characters)\n");

    ConcurrentCommandLine::writeLine(helpMessage);
}

void MessageWriter::writeLoginMessage(std::string profileId)
{
    auto standardProfileId = CommandLineParser::standardizeProfileId(profileId);
    auto loginMessage = appName_ + string(": logged in as ") + standardProfileId + string(".");

    ConcurrentCommandLine::writeLine(loginMessage);
}

void MessageWriter::writeTimelineHeader()
{
    auto timelineHeader = string("\n--- Your timeline ---\n");
    ConcurrentCommandLine::writeLine(timelineHeader);
}

void MessageWriter::writeInvalidCommandMessage(std::string invalidCommand)
{
    auto errorMessage = appName_ + string(": '") + invalidCommand + string("' is not a valid command.\n");
    ConcurrentCommandLine::writeLine(errorMessage);
}

void MessageWriter::writeServerNackMessage()
{
    auto errorMessage = appName_ + string(": Could not acknowledge your latest command. Please try again.\n");
    ConcurrentCommandLine::writeLine(errorMessage);
}
