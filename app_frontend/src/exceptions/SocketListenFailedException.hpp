/**
 * Thrown when failed to bind socket descritor.
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
        class SocketListenFailedException : public std::exception
        {
        public:
            SocketListenFailedException(int socketDescriptor)
            {
                this->socketDescriptor_ = socketDescriptor;
            }

            virtual const char* what() const throw()
            {
                return std::string("Failed to start listening for connections on socket " + socketDescriptor_).c_str();
            }

        private:
            std::string socketDescriptor_;
        };
    }
}
