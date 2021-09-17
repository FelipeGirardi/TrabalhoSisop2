#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include <list>
#include "Session.hpp"
#include "../../common/include/Notification.hpp"
#include "../../common/include/Packet.hpp"

using namespace notification;

namespace communicationManager {

    class CommunicationManager {
    private:
        int sendPacketToSessions(list<Session> sessions, Packet *package);
    public:
        int read_text(int socket, char *buffer);
        int read_packet(int socket, Packet *package, char *buffer);
        int send_packet(int socket, Packet *package);
        int sendNotificationToSessions(list<Session> sessions, Notification notification);
        Packet createAckPacketForType(PacketType type);
        Packet createGenericNackPacket();
        string stringDescribingType(PacketType type);

    };
}


#endif //COMMUNICATIONMANAGER_H
