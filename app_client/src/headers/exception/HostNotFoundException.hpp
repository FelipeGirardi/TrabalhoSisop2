/**
 * Thrown when host address cannot be found.
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
        class HostNotFoundException : public std::exception
        {
        public:
            HostNotFoundException(std::string host)
            {
                this->host_ = host;
            }

            virtual const char* what() const throw()
            {
                return std::string("Host address '" + host_ + "' cannot be found!").c_str();
            }

        private:
            std::string host_;
        };
    }
}
