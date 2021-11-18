/**
 * Thrown when socket cannot be created.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <exception>

namespace ClientApp
{
    namespace Exception
    {
        class SocketNotCreatedException : public std::exception
        {
        public:
            virtual const char* what() const throw()
            {
                return "Socket cannot be created!";
            }
        };
    }
}
