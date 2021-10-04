/**
 * Thrown when socket read an unexpected packet type.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

#include "PacketType.hpp"
#include <exception>
#include <string>

namespace ClientApp
{
    namespace Exception
    {
        class UnexpectedPacketTypeException : public std::exception
        {
        public:
            UnexpectedPacketTypeException(PacketType expected, PacketType actual)
            {
                this->_expectedType = expected;
                this->_actualType = actual;
            }

            virtual const char* what() const throw()
            {
                auto message = std::string("Expected packet type ") +
                    std::to_string(_expectedType) +
                    std::string(" , but got packet type ") +
                    std::to_string(_actualType);

                return message.c_str();
            }

        private:
            PacketType _expectedType;
            PacketType _actualType;
        };
    }
}
