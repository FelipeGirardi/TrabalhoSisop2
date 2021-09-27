/**
 * Parses command line arguments.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "config/GlobalConstants.hpp"
#include "io/CommandLineParser.hpp"
#include "io/ConcurrentCommandLine.hpp"
#include "exception/InvalidArgsException.hpp"
#include "exception/InvalidCommandException.hpp"
#include "Packet.hpp"
#include "StringExtensions.hpp"
#include <cstring>

using namespace ClientApp::IO;
using namespace std;

using ClientApp::Exception::InvalidArgsException;
using ClientApp::Exception::InvalidCommandException;
using Common::StringExtensions;

// Public methods

ClientAppArgs CommandLineParser::parseClientAppArgs(int argc, char** argv)
{
    if ((argc <= PORT_INDEX) || (argv == nullptr))
    {
        ConcurrentCommandLine::writeLine("usage: ./app_client [profile] [server-host] [server-port]");
        throw InvalidArgsException();
    }

    ClientAppArgs parsedArgs = {
        argv[PROFILE_ID_INDEX],
        argv[HOST_INDEX],
        argv[PORT_INDEX]
    };

    return parsedArgs;
}

Packet CommandLineParser::parseClientAppCommand(std::string userCommand)
{
    auto splitIndex = userCommand.find_first_of(' ');
    if (splitIndex == string::npos)
        throw InvalidCommandException();

    auto commandType = parseCommandType(userCommand.substr(0, splitIndex));
    auto commandArgs = parseCommandArgs(commandType, userCommand.substr(splitIndex + 1));
    auto outboundPacket = Packet{ commandType, (int)commandArgs.length(), (long)time(NULL) };
    strcpy(outboundPacket._payload, commandArgs.c_str());

    return outboundPacket;
}

string CommandLineParser::standardizeProfileId(string profileId)
{
    auto standardProfileId = string(profileId);
    if (!standardProfileId.empty() && standardProfileId[0] != '@')
        standardProfileId = string("@") + standardProfileId;

    return standardProfileId;
}

// Private methods

PacketType CommandLineParser::parseCommandType(std::string commandType)
{
    auto uppercaseCommandType = StringExtensions::toUppercase(commandType);

    if (uppercaseCommandType == string("SEND"))
        return SEND;
    if (uppercaseCommandType == string("FOLLOW"))
        return FOLLOW;

    throw InvalidCommandException();
}

string CommandLineParser::parseCommandArgs(PacketType commandType, string commandArgs)
{
    auto parsedArgs = string(commandArgs);

    if (commandType == FOLLOW)
    {
        parsedArgs = standardizeProfileId(parsedArgs);

        auto profileLength = parsedArgs.length();
        if (profileLength < PROFILE_MIN_SIZE || profileLength > PROFILE_MAX_SIZE)
            throw InvalidCommandException();
    }

    return parsedArgs;
}
