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
#include "../include/utils/ErrorCodes.hpp"
#define PORT 4000

using namespace std;
using namespace notification;

namespace communicationManager {
    ErrorCodes CommunicationManager::send_packet(int socket, Packet* package) {
        cout << "Enviando o pacote:" << endl;
        package->printItself();

        if (write(socket, package, sizeof(Packet)) < 0) {
            cout << "ERRO escrevendo no socket" << endl;
            return ERROR;
        }
        return SUCCESS;
    }

    /*
     * retorna SUCCESS quando conseguiu enviar a pelo menos uma das sessões do usuário
     * retorna ERROR caso contrário
     */
    ErrorCodes CommunicationManager::sendPacketToSessions(list<Session> sessions, Packet* package) {

        ErrorCodes returnValue = ERROR;
        for (Session session : sessions) {

            cout << "Enviando pacote para " << session.userID << "socket = " << session.notif_socket << endl;
            if (this->send_packet(session.notif_socket, package) >= 0) {
                returnValue = SUCCESS;
            }
        }
        return returnValue;
    }

    /*
     * retorna SUCCESS quando conseguiu enviar a pelo menos uma das sessões do usuário
     * retorna ERROR caso contrário
     */
    ErrorCodes CommunicationManager::sendNotificationToSessions(list<Session> sessions, Notification notification) {

        cout << "Notificação sendo enviada " << notification.toString() << endl;

        Packet* packet = new Packet;
        packet->timestamp = time(NULL);
        packet->type = NOTIFICATION;

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

        cout << "Criando pacote ACK" << endl;
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

        cout << "Criando pacote NACK" << endl;
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
