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
#include "../../common/include/Notification.hpp"
#include "../../common/include/PacketType.hpp"
#include "../include/utils/ErrorCodes.hpp"

using namespace std;
using namespace notification;

namespace communicationManager {


    ErrorCodes CommunicationManager::send_packet(int socket, Packet* package) {
        cout << "Enviando o pacote:" << endl;
        package->printItself();

        int aa = write(socket, package, sizeof(Packet));
        cout << "valor de retorno do write = " << aa << endl;

        if ( aa < 0) {
            cout << "ERRO escrevendo no socket" << endl;
            return ERROR;
        }
        return SUCCESS;
    }

    /*
     * retorna SUCCESS quando conseguiu enviar a pelo menos uma das sessões do usuário.
     * Se lista de sessões está vazia, é considerado sucesso.
     * retorna ERROR caso contrário
     */
    ErrorCodes CommunicationManager::sendPacketToSessions(list<Session> sessions, Packet* package) {

        cout << "Enviando o pacote de " << stringDescribingType(package->type) <<
        " para o total de " << sessions.size() << " sessões." << endl;

        if (sessions.empty()) { return SUCCESS; }

        ErrorCodes returnValue = ERROR;
        for (Session session : sessions) {

            cout << "Enviando pacote para " << session.userID << " socket = " << session.notif_socket << endl;
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

    Packet CommunicationManager::createAckPacketForType(PacketType type) {

        cout << "Criando pacote ACK" << endl;
        string responseString = "Uhu! " + stringDescribingType(type) + " recebido com sucesso! :)";

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
    Packet CommunicationManager::createEmptyPacket(PacketType type) {

        cout << "Criando pacote KEEP ALIVE" << endl;

        Packet package;
        package.type = type;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        package.length = strlen(package._payload);
        return package;
    }

    Packet CommunicationManager::createPacketWithID(int _id, PacketType type) {
        cout << "Criando pacote contendo id = " << _id << endl;

        Packet package;
        package.type = type;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        strncpy(package._payload, to_string(_id).c_str(), BUFFER_SIZE);
        package.length = strlen(package._payload);
        return package;
    }

    Packet CommunicationManager::createCoordinatorPacket(int idCurrentProcess) {

        cout << "Criando pacote COORDINATOR " << endl;
        return createPacketWithID(idCurrentProcess, COORDINATOR);
    }

    Packet CommunicationManager::createHelloPacket(int idCurrentProcess) {

        cout << "Criando pacote HELLO " << endl;
        return createPacketWithID(idCurrentProcess, HELLO_SERVER);
    }

    /*
     * Creates exit packet. Sent when server receives a control C.
     * The payload is a buffer with BUFFER_SIZE zeros
     */
    Packet CommunicationManager::createExitPacket() {

        cout << "Criando pacote EXIT" << endl;

        Packet package;
        package.type = EXIT;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        package.length = BUFFER_SIZE;
        return package;
    }

}
