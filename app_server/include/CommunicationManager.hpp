#ifndef COMMUNICATIONMANAGER_HPP
#define COMMUNICATIONMANAGER_HPP

#include <list>
#include "Session.hpp"
#include "../../common/include/Notification.hpp"
#include "../../common/include/Packet.hpp"
#include "../include/utils/ErrorCodes.hpp"

using namespace notification;

namespace communicationManager {

    class CommunicationManager {
    public:
        ErrorCodes send_packet(int socket, Packet *package);
        ErrorCodes sendNotificationToSessions(list<Session> sessions, Notification notification);
        ErrorCodes sendPacketToSessions(list<Session> sessions, Packet *package);
        Packet createAckPacketForType(PacketType type);
        Packet createGenericNackPacket();
        Packet createEmptyPacket(PacketType type);
        Packet createExitPacket();

    };
}


#endif //COMMUNICATIONMANAGER_HPP
