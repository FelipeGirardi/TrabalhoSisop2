/**
 * Writes common alert messages.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <string>

namespace ClientApp
{
    namespace Service
    {
        class MessageWriter
        {
        public:
            MessageWriter(std::string appName);

            void writeHelpMessage();
            void writeLoginMessage(std::string profileId);
            void writeTimelineHeader();
            void writeInvalidCommandMessage(std::string invalidCommand);
            void writeServerNackMessage();

        private:
            const std::string appName_;
        };
    }
}
