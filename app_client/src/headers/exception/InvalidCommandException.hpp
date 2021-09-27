/**
 * Thrown when invalid commands are entered by the user.
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
        class InvalidCommandException : public std::exception
        {
        public:
            virtual const char* what() const throw()
            {
                return "Invalid command!";
            }
        };
    }
}
