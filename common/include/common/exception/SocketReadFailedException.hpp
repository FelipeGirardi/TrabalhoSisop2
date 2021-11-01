/**
 * Thrown when socket failed to read data.
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
        class SocketReadFailedException : public std::exception
        {
        public:
            SocketReadFailedException(const int socketDescriptor)
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
