/**
 * Parses command line arguments.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "Packet.hpp"
#include <string>

namespace ClientApp
{
    namespace IO
    {
        struct ClientAppArgs
        {
            std::string profileId;
            std::string host;
            std::string port;
        };

        enum ClientAppArgsIndex
        {
            PROFILE_ID_INDEX = 1,
            HOST_INDEX = 2,
            PORT_INDEX = 3
        };

        class CommandLineParser
        {
        public:
            static ClientAppArgs parseClientAppArgs(int argc, char** argv);
            static Packet parseClientAppCommand(std::string userCommand);
            static std::string standardizeProfileId(std::string profileId);

        private:
            static PacketType parseCommandType(std::string commandType);
            static std::string parseCommandArgs(PacketType commandType, std::string commandArgs);
        };
    }
}
