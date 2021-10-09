/**
 * Unit tests for Notification class.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "SessionAuth.hpp"
#include <iostream>

using namespace Common;
using namespace std;

namespace Common
{
    namespace Tests
    {
        void constructor_WithoutUuid_GeneratesUuid();
        void constructor_WithAllFields_UsesArguments();
        void fromBytes_toBytes_ConvertsBetweenBytesAndClass();
    }
}

int main()
{
    void (*testFunctions[])() = {
        Common::Tests::constructor_WithoutUuid_GeneratesUuid,
        Common::Tests::constructor_WithAllFields_UsesArguments,
        Common::Tests::fromBytes_toBytes_ConvertsBetweenBytesAndClass,
    };

    for (auto testFunction : testFunctions)
        testFunction();
}

// Implementations

void Common::Tests::constructor_WithoutUuid_GeneratesUuid()
{
    char profileId[] = "@unit-test";
    auto sessionAuth = SessionAuth(profileId, NOTIFICATION_SOCKET);

    cout << "constructor_WithoutUuid_GeneratesUuid\r\n"
        << "    profileId = " << sessionAuth.getProfileId() << "\r\n"
        << "         uuid = " << sessionAuth.getUuid() << "\r\n"
        << "   socketType = " << sessionAuth.getSocketType() << "\r\n"
        << "\r\n";
}

void Common::Tests::constructor_WithAllFields_UsesArguments()
{
    char pretestProfileId[] = "@pre-test";
    auto sessionAuthWithoutUuid = SessionAuth(pretestProfileId, COMMAND_SOCKET);

    char profileId[] = "@unit-test";
    auto sessionAuthWithUuid = SessionAuth(profileId, NOTIFICATION_SOCKET, (char*)sessionAuthWithoutUuid.getUuid().c_str());

    cout << "constructor_WithAllFields_UsesArguments\r\n"
        << "    expected uuid = " << sessionAuthWithoutUuid.getUuid() << "\r\n"
        << "    profileId = " << sessionAuthWithUuid.getProfileId() << "\r\n"
        << "         uuid = " << sessionAuthWithUuid.getUuid() << "\r\n"
        << "   socketType = " << sessionAuthWithUuid.getSocketType() << "\r\n"
        << "\r\n";
}

void Common::Tests::fromBytes_toBytes_ConvertsBetweenBytesAndClass()
{
    char pretestProfileId[] = "@unit-test";
    auto pretestSessionAuth = SessionAuth(pretestProfileId, NOTIFICATION_SOCKET);

    auto bytes = pretestSessionAuth.toBytes();
    auto sessionAuth = SessionAuth::fromBytes(bytes);

    cout << "fromBytes_toBytes_ConvertsBetweenBytesAndClass\r\n"
        << "    profileId = " << sessionAuth->getProfileId() << "\r\n"
        << "         uuid = " << sessionAuth->getUuid() << "\r\n"
        << "   socketType = " << sessionAuth->getSocketType() << "\r\n"
        << "\r\n";
}
