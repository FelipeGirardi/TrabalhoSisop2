/**
 * Thrown when profile authentication has failed.
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
        class AuthenticationFailedException : public std::exception
        {
        public:
            AuthenticationFailedException(std::string profileId)
            {
                this->profileId_ = profileId;
            }

            virtual const char* what() const throw()
            {
                return std::string("Authentication failed for '" + profileId_ + "'").c_str();
            }

        private:
            std::string profileId_;
        };
    }
}
