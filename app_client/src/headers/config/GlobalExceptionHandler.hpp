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
            static void enable();
            static void handleUncaughtException();
        };
    }
}
