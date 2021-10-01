/**
 * Defines session authorizations.
 *
 * \author Renan Kummer
 */
#pragma once

#include <string>

namespace Common
{
    enum SocketType
    {
        NOTIFICATION_SOCKET,
        COMMAND_SOCKET
    };

    class SessionAuth
    {
    public:
        SessionAuth();
        SessionAuth(char profileId[21], SocketType type);
        SessionAuth(char profileId[21], SocketType type, char uuid[37]);

        std::string getProfileId();
        std::string getUuid();
        SocketType getSocketType();

        static SessionAuth* fromBytes(char* sessionAuthInBytes);
        char* toBytes();

    private:
        char _profileId[21];
        SocketType _socketType;
        char _uuid[37];
    };
}
