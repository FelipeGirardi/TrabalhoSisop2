/**
 * Remote proxy for interactions with the server.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#include "common/network/SocketConnection.hpp"
#include "service/ServerProxy.hpp"

using namespace std;
using namespace Common::Model;
using namespace Common::Network;
using namespace Common::Exception;
using namespace FrontendApp::Service;

// Static fields
unique_ptr<Socket> ServerProxy::primaryServerSocket_ = nullptr;
ServerUrl ServerProxy::primaryServerUrl_ = { "", 0 };
bool ServerProxy::hasNewPrimaryServer_ = false;
mutex ServerProxy::primaryServerMutex_;

// Constructors
ServerProxy::ServerProxy(Socket senderSocket, Socket listenerSocket)
    : isConnected_(true),
    senderSocket_(make_unique<Socket>(senderSocket)),
    listenerSocket_(make_unique<Socket>(listenerSocket)) {}

// Public static functions
ServerProxy ServerProxy::connect()
{
    auto hostEntry = createHostEntry(primaryServerUrl_.host);
    auto senderSocketDescriptor = createSocketDescriptor();
    auto listenerSocketDescriptor = createSocketDescriptor();

    auto senderSocket = connectSocketToHost(senderSocketDescriptor, hostEntry, primaryServerUrl_.port);
    auto listenerSocket = connectSocketToHost(listenerSocketDescriptor, hostEntry, primaryServerUrl_.port);

    auto serverProxy = ServerProxy(senderSocket, listenerSocket);
    return serverProxy;
}

void ServerProxy::listenPrimaryServerChanges()
{
    // TODO: Not implemented
}

ServerUrl ServerProxy::onPrimaryServerChange()
{
    while (!hasPrimaryServerChanged()) {}

    primaryServerMutex_.lock();
    hasNewPrimaryServer_ = false;
    primaryServerMutex_.unlock();

    return primaryServerUrl_;
}

// Public functions
void ServerProxy::send(const Packet& packet)
{
    if (isConnected() && !hasPrimaryServerChanged())
        senderSocket_->send(packet);
}

unique_ptr<Packet> ServerProxy::listen()
{
    if (isConnected() && !hasPrimaryServerChanged())
        return listenerSocket_->receive();

    return nullptr;
}

void ServerProxy::disconnect()
{
    isConnected_ = false;

    Packet exitPacket = { EXIT, 256, 0, "EXIT" };
    senderSocket_->send(exitPacket);

    closeSocket(listenerSocket_->getDescriptor());
    closeSocket(senderSocket_->getDescriptor());
}

// Private static functions
bool ServerProxy::hasPrimaryServerChanged()
{
    return hasNewPrimaryServer_;
}

// Private functions
bool ServerProxy::isConnected()
{
    return isConnected_;
}
