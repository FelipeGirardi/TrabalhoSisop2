/**
 * Handler for signals raised in runtime.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

namespace ClientApp
{
    namespace Config
    {
        class GlobalSignalHandler
        {
        public:
            static bool enable();
            static void handleInteractiveAttentionSignal(int signalNum);
            static void handleAbortSignal(int signalNum);
        };
    }
}
