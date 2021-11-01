/**
 * Definition of a client session.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "model/ClientSession.hpp"

using namespace std;
using namespace FrontendApp::Model;


// Public functions

ClientSession::ClientSession(
    int commandSocketDescriptor,
    int notificationSocketDescriptor,
    std::string clientId,
    std::string sessionUuid
)
    : commandSocket_(commandSocketDescriptor),
    notificationSocket_(notificationSocketDescriptor),
    clientId_(clientId),
    sessionUuid_(sessionUuid) { }

unique_ptr<const Socket> getCommandSocket()
{
    auto commandSocket = unique_ptr<const Socket>(&commandSocket_);
    return commandSocket;
}

unique_ptr<const Socket> getNotificationSocket()
{
    auto notificationSocket = unique_ptr<const Socket>(&notificationSocket_);
    return notificationSocket;
}

const string getClientId()
{
    return clientId_;
}

const string getSessionUuid()
{
    return sessionUuid_;
}
