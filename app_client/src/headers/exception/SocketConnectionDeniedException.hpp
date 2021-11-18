/**
 * Thrown when socket connection cannot be established.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <exception>
#include <string>

namespace ClientApp
{
    namespace Exception
    {
        class SocketConnectionDeniedException : public std::exception
        {
        public:
            virtual const char* what() const throw()
            {
                return "Socket connection cannot be established!";
            }
        };
    }
}
