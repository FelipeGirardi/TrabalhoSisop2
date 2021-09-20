/**
 * Parses command line arguments.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <string>

namespace ClientApp
{
    namespace IO
    {
        typedef struct ClientAppArgs
        {
            std::string profileId;
            std::string host;
            std::string port;
        } ClientAppArgs;

        typedef enum ClientAppArgsIndex
        {
            PROFILE_ID_INDEX = 1,
            HOST_INDEX = 2,
            PORT_INDEX = 3
        } ClientAppArgsIndex;

        class CommandLineParser
        {
        public:
            static ClientAppArgs parseClientAppArgs(int argc, char** argv);
        };
    }
}
