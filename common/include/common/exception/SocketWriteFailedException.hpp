/**
 * Thrown when socket failed to send data.
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
        class SocketWriteFailedException : public std::exception
        {
        public:
            SocketWriteFailedException(const int socketDescriptor)
            {
                this->socketDescriptor_ = socketDescriptor;
            }

            virtual const char* what() const throw()
            {
                return std::string("Could not write data to socket " + socketDescriptor_).c_str();
            }

        private:
            std::string socketDescriptor_;
        };
    }
}
