/**
 * Parses command line arguments.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "Packet.hpp"

#include <string>

namespace FrontendApp
{
    namespace IO
    {
        struct FrontendArgs
        {
            std::string host;
            std::string clientPort;
            std::string serverPort;
        };

        class CommandLineParser
        {
        public:
            static FrontendArgs parseArgs(int argc, char** argv);

        private:
            static unsigned getArgsMinSize();

            static const unsigned hostIndex_;
            static const unsigned clientPortIndex_;
            static const unsigned serverPortIndex_;
        };
    }
}
