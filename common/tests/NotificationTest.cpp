/**
 * Unit tests for Notification class.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "Notification.hpp"
#include <iostream>

using namespace notification;
using std::cout;

namespace Common
{
    namespace Tests
    {
        void toBytes_fromBytes_InitializedObject_ReturnsObjectInBytes();
    }
}

int main()
{
    void (*testFunctions[])() = {
        Common::Tests::toBytes_fromBytes_InitializedObject_ReturnsObjectInBytes
    };

    for (auto testFunction : testFunctions)
    {
        testFunction();
    }
}

// Implementations

void Common::Tests::toBytes_fromBytes_InitializedObject_ReturnsObjectInBytes()
{
    auto expectedNotification = Notification("id", "notification text", "username", 0, 1);

    auto bytes = expectedNotification.toBytes();

    auto parsedNotification1 = Notification::parseBytes(bytes);
    auto parsedNotification2 = Notification::parseBytes(bytes);
    parsedNotification2->setID("another id");

    cout << "toBytes_fromBytes_InitializedObject_ReturnsObjectInBytes\r\n"
        << "   id = " << parsedNotification1->getID() << "\r\n"
        << "   text = " << parsedNotification1->getText() << "\r\n"
        << "   username = " << parsedNotification1->getUsername() << "\r\n"
        << "   time = " << parsedNotification1->getTime() << "\r\n"
        << "   pendingReaders = " << parsedNotification1->getPendingReaders() << "\r\n"
        << "   parsedNotification1 != parsedNotification2 -> " << (parsedNotification1->getID() != parsedNotification2->getID()) << "\r\n"
        << "\r\n";
}
