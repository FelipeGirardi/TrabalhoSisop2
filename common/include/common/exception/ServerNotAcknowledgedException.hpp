/**
 * Thrown when server doesn't acknowledge client request.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <exception>
#include <string>

namespace Common
{
    namespace Exception
    {
        class ServerNotAcknowledgedException : public std::exception
        {
        public:
            ServerNotAcknowledgedException(const int socketDescriptor)
                : socketDescriptor_(socketDescriptor) { }

            virtual const char* what() const throw()
            {
                return std::string("Server didn't acknowledge request from socket " + socketDescriptor_).c_str();
            }

        private:
            const std::string socketDescriptor_;
        };
    }
}
