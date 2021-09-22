/**
 * Thrown when function arguments are invalid.
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
        class InvalidArgsException : public std::exception
        {
        public:
            virtual const char* what() const throw()
            {
                return "Invalid function arguments!";
            }
        };
    }
}
