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
    private:
        ErrorCodes sendPacketToSessions(list<Session> sessions, Packet *package);
    public:
        ErrorCodes send_packet(int socket, Packet *package);
        ErrorCodes sendNotificationToSessions(list<Session> sessions, Notification notification);
        Packet createAckPacketForType(PacketType type);
        Packet createGenericNackPacket();
        string stringDescribingType(PacketType type);

    };
}


#endif //COMMUNICATIONMANAGER_HPP
