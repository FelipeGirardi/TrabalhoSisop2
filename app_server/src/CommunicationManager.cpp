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
#include <iostream>
#include <list>
#include "../include/Session.hpp"
#include "../include/CommunicationManager.hpp"
#include "../include/Session.hpp"
#include "../../Common/include/Notification.hpp"
#define PORT 4000

using namespace std;
using namespace notification;

namespace communicationManager {
    int read_text(int socket, char* buffer) {
        //    int n;
        //    n = read(socket, buffer, BUFFER_SIZE);
        //    if (n < 0)
        //        printf("ERROR reading from socket");
        //
        //    return n;
        return 0; // Avoid compilation warning.
    }

    int read_packet(int socket, Packet* package, char* buffer) {
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
        return 0; // Avoid compilation warning.
    }

    int CommunicationManager::send_packet(int socket, Packet* package) {
        int n;
        package->printItself();

        n = write(socket, package, sizeof(Packet));
        if (n < 0) {
            printf("ERROR writing to socket\n");
            n = -1;
        }
        return n;
    }

    /*
     * retorna 1 quando conseguiu enviar a pelo menos uma das sessões do usuário
     * retorna 0 caso contrário
     */
    int CommunicationManager::sendPacketToSessions(list<Session> sessions, Packet* package) {
        cout << "inside sendPacketToSessions. Number of sessions:" << sessions.size() << endl;
        int returnValue = 0;
        for (Session session : sessions) {

            cout << "enviando pacote para " << session.userID << " " << session.notif_socket << endl;
            if (this->send_packet(session.notif_socket, package) >= 0) {
                returnValue = 1;
            }
        }
        return returnValue;
    }

    /*
     * retorna 1 quando conseguiu enviar a pelo menos uma das sessões do usuário
     * retorna 0 caso contrário
     */
    int CommunicationManager::sendNotificationToSessions(list<Session> sessions, Notification notification) {

        cout << "inside sendNotificationToSessions function" << endl;
        cout << "notification to send" << notification.toString() << endl;

        Packet* packet = new Packet;
        packet->timestamp = time(NULL);
        packet->type = NOTIFICATION;

        cout << "created packet" << endl;
        string notString = notification.toString();
        notString.pop_back();
        const char* cstr = notString.c_str();

        bzero(packet->_payload, BUFFER_SIZE);
        strncpy(packet->_payload, cstr, BUFFER_SIZE);
        packet->length = sizeof(packet->_payload);

        return sendPacketToSessions(sessions, packet);
    }

    string CommunicationManager::stringDescribingType(PacketType type) {
        switch (type) {
        case SEND:
            return "SEND";
        case FOLLOW:
            return "FOLLOW";
        case EXIT:
            return "EXIT";
        case USERNAME:
            return "USERNAME";
        default:
            return "";
        }
    }

    Packet CommunicationManager::createAckPacketForType(PacketType type) {

        cout << "inside creating ACK packet" << endl;
        string responseString = "Uhu! " + this->stringDescribingType(type) + " recebido com sucesso! :)";

        Packet package;
        package.type = SERVER_ACK;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        strncpy(package._payload, responseString.c_str(), BUFFER_SIZE);
        package.length = strlen(package._payload);
        return package;
    }

    Packet CommunicationManager::createGenericNackPacket() {

        cout << "inside creating NACK packet" << endl;
        string responseString = "Erro!";

        Packet package;
        package.type = SERVER_ERROR;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        strncpy(package._payload, responseString.c_str(), BUFFER_SIZE);
        package.length = strlen(package._payload);
        return package;
    }

}
