/**
 * Thrown when unexpected packet type is received.
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
        class UnexpectedPacketTypeException : public std::exception
        {
        public:
            virtual const char* what() const throw()
            {
                return "An unexpected packet type has been received!";
            }
        };
    }
}
