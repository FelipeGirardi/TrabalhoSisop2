#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <list>
#include "../include/Session.hpp"
#include "../include/CommunicationManager.hpp"
#include "../include/ClientAuthData.hpp"
#include "../../Common/include/Notification.hpp"
#define PORT 4000

using namespace std;
using namespace notification;

namespace communicationManager {
    int read_text(int socket, char *buffer) {
        //    int n;
        //    n = read(socket, buffer, BUFFER_SIZE);
        //    if (n < 0)
        //        printf("ERROR reading from socket");
        //
        //    return n;
    }

    int read_packet(int socket, Packet *package, char *buffer) {
        //    int n;
        //    char payload[128]="";
        //
        //    // Lendo bytestream
        //    n = read(socket, buffer, BUFFER_SIZE);
        //    if (n < 0) {
        //        printf("ERROR reading from socket\n");
        //        n = -1;
        //    }
        //    // ..
        //
        //    // Montando pacote
        //    package->type = buffer[0] | buffer[1] << 8;
        //    package->seqn = buffer[2] | buffer[3] << 8;
        //    package->length = buffer[4] | buffer[5] << 8;
        //    //package->timestamp = buffer [8] | buffer[9] | buffer[10] | buffer[11] | buffer [12] | buffer [13] | buffer[14] | buffer[15];
        //    package->_payload = payload;
        //
        //    // Montando payload
        //    for(int i = 0; i < 7; i++) {
        //        payload[i] = buffer[i];
        //    }
        //    printf("Here is the message: %s\n", payload);
        //
        //    return n;
    }

    int CommunicationManager::send_packet(int socket, Packet *package) {
        int n;
        // Enviando metadados
        n = write(socket, package, sizeof(Packet));
        if (n < 0){
            printf("ERROR writing metadata to socket\n");
            n = -1;
        }
        // Enviando payload
        n = write(socket, package->_payload, package->length);
        if (n < 0){
            printf("ERROR writing data to socket\n");
            n = -1;
        }
        return n;
    }

    int CommunicationManager::sendPacketToSessions(list<Session> sessions, Packet *package) {
        int returnValue = 0;
        for (Session session : sessions) {
            if (this->send_packet(session.commandSocket, package) >= 0) {
                returnValue ++;
            }
        }
        return returnValue;
    }

    int CommunicationManager::sendNotificationToSessions(list<Session> sessions, Notification notification) {

        Packet *packet;
        packet->length = 0;
        packet->seqn = 0;
        packet->timestamp = 0;
        packet->type = 0;

        const char *cstr = notification.toString().c_str();
        strncpy(packet->_payload, cstr, BUFFER_SIZE);

        return sendPacketToSessions(sessions, packet);

    }

}