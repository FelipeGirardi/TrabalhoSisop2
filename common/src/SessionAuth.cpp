/**
 * Defines session authorizations.
 *
 * \author Renan Kummer
 */
#include "SessionAuth.hpp"
#include <cstring>
#include <uuid/uuid.h>

using namespace Common;
using std::string;

SessionAuth::SessionAuth(char profileId[21], SocketType type)
{
    strncpy(this->_profileId, profileId, 21);
    this->_socketType = type;

    uuid_t id;
    uuid_generate(id);
    uuid_unparse(id, this->_uuid);
}

SessionAuth::SessionAuth(char profileId[21], SocketType type, char uuid[37])
{
    strncpy(this->_profileId, profileId, 21);
    this->_socketType = type;
    strncpy(this->_uuid, uuid, 37);
}

// Public methods

string SessionAuth::getProfileId()
{
    return string(_profileId);
}

string SessionAuth::getUuid()
{
    return string(_uuid);
}

SocketType SessionAuth::getSocketType()
{
    return _socketType;
}

SessionAuth* SessionAuth::fromBytes(char* sessionAuthInBytes)
{
    auto parsedSessionAuth = (SessionAuth*)sessionAuthInBytes;

    auto sessionAuthCopy = new SessionAuth(
        (char*)parsedSessionAuth->getProfileId().c_str(),
        parsedSessionAuth->getSocketType(),
        (char*)parsedSessionAuth->getUuid().c_str()
    );

    return sessionAuthCopy;
}

long SessionAuth::sizeInBytes()
{
    return (sizeof(char) * 21) + sizeof(SocketType) + (sizeof(char) * 37);
}

char* SessionAuth::toBytes()
{
    auto sessionAuthCopy = new SessionAuth(_profileId, _socketType, _uuid);
    return (char*)sessionAuthCopy;
}
