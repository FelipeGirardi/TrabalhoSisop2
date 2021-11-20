/**
 * Thrown when function arguments are invalid.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <exception>

namespace FrontendApp
{
    namespace Exception
    {
        class InvalidCommandLineArgsException : public std::exception
        {
        public:
            virtual const char* what() const throw()
            {
                return "usage: ./app_frontend [server-host] [client-port] [server-port]";
            }
        };
    }
}
