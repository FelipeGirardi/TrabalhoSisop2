/**
 * Thread-safe wrapper for command line operations.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <string>
#include <mutex>

namespace ClientApp
{
    namespace IO
    {
        class ConcurrentCommandLine
        {
        public:
            static bool initialize();
            static bool terminate();
            static std::string readLine(int maxChars);
            static void writeLine(std::string output);
            static void waitInput();

        private:
            static bool isInitialized();
            static void setInitialized(bool isInitialized);
            static void showInput();
            static void hideInput();

            static bool isInitialized_;
            static std::mutex cliMutex_;
        };
    }
}
