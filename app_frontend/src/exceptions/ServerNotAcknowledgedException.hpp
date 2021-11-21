/**
 * Thrown when server doesn't acknowledge client request.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include <string>
#include <exception>

namespace FrontendApp
{
    namespace Exception
    {
        class ServerNotAcknowledgedException : public std::exception
        {
        public:
            ServerNotAcknowledgedException(int socketDescriptor)
            {
                this->socketDescriptor_ = socketDescriptor;
            }

            virtual const char* what() const throw()
            {
                return std::string("Server didn't acknowledge request from socket " + socketDescriptor_).c_str();
            }

        private:
            std::string socketDescriptor_;
        };
    }
}
