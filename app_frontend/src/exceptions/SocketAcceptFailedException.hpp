/**
 * Thrown when failed to accept socket connection attempt.
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
        class SocketAcceptFailedException : public std::exception
        {
        public:
            SocketAcceptFailedException(int socketDescriptor, bool isClientAttempt = true)
            {
                this->socketDescriptor_ = socketDescriptor;
                this->isClientAttempt_ = isClientAttempt;
            }

            virtual const char* what() const throw()
            {
                if (isClientAttempt_)
                    return std::string("Failed to accept client connection from socket " + socketDescriptor_).c_str();

                return std::string("Failed to accept server connection from socket " + socketDescriptor_).c_str();
            }

        private:
            std::string socketDescriptor_;
            bool isClientAttempt_;
        };
    }
}
