/**
 * Handler for uncaught exceptions in runtime.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

namespace ClientApp
{
    namespace Config
    {
        class GlobalExceptionHandler
        {
        public:
            static bool enable();
            static void handleUncaughtException();
        };
    }
}
