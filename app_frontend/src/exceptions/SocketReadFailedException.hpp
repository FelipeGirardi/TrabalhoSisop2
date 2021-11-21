/**
 * Thrown when socket failed to read data from server.
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
        class SocketReadFailedException : public std::exception
        {
        public:
            SocketReadFailedException(int socketDescriptor)
            {
                this->socketDescriptor_ = socketDescriptor;
            }

            virtual const char* what() const throw()
            {
                return std::string("Could not read data from socket " + socketDescriptor_).c_str();
            }

        private:
            std::string socketDescriptor_;
        };
    }
}
