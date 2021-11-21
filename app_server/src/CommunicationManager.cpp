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
#include "../../common/include/FrontEndPayload.hpp"
#include "../include/utils/ErrorCodes.hpp"
#include "../include/ServerAndFrontEndInfo.h"
#include "../include/FrontEndCommunicationManager.hpp"
#include "../include/GlobalManager.hpp"

using namespace std;
using namespace notification;

namespace communicationManager {


    ErrorCodes CommunicationManager::send_packet(int socket, Packet* package) {
        //cout << "Enviando o pacote:" << endl;
        //package->printItself();

        int result = write(socket, package, sizeof(Packet));

        if (result < 0) {
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

    ErrorCodes CommunicationManager::sendPacketToSockets(list<int> sockets, Packet* package) {

        cout << "Enviando o pacote de " << stringDescribingType(package->type) <<
             " para o total de " << sockets.size() << " sockets." << endl;

        if (sockets.empty()) { return SUCCESS; }

        ErrorCodes returnValue = ERROR;
        for (int socket : sockets) {

            cout << "Enviando pacote para socket" << socket << endl;
            if (this->send_packet(socket, package) >= 0) {
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

    ErrorCodes CommunicationManager::sendPacketToRMS(Packet *packet) {

        cout << "Replicando pacote para RMS" << endl;

        vector<ServerInfo> rms = GlobalManager::electionManager.getServers();
        ErrorCodes successCode;
        int currentServerID = GlobalManager::electionManager.getProcessID();

        for (auto rm : rms) {
            if (rm._id == currentServerID ||  rm.sendSocket == INVALID_SOCKET) { continue; }
            cout << "Enviando pacote para rm de id = " << rm._id << " ";
            cout << "IP = "<< rm.ip << " ";
            cout << "send socket = " <<  rm.sendSocket << endl;
            successCode = send_packet(rm.sendSocket, packet);
            if (successCode == SUCCESS) {
                cout << "SUCESSO enviando!" << endl;
            } else {
                cout << "ERRO enviando!" << endl;
            }
        }

        return SUCCESS;

    }

    Packet *CommunicationManager::createNotificationPacket(string username, Notification notification) {
        Packet* packet = new Packet;
        packet->timestamp = time(NULL);
        packet->type = NOTIFICATION;
        packet->length = sizeof (FrontEndPayload);

        FrontEndPayload *frontEndPayload = new FrontEndPayload;

        strncpy(frontEndPayload->commandContent, notification.toString().c_str(), 100);
        strncpy(frontEndPayload->senderUsername, username.c_str(), 100);

        memcpy(packet->_payload, frontEndPayload->toBytes(), BUFFER_SIZE);
        return packet;
    }

    // Cria pacote contendo no payload um username e o id da notificacao
    Packet *CommunicationManager::createNotificationIDPacket(string username, string notificationID) {
        Packet* packet = new Packet;
        packet->timestamp = time(NULL);
        packet->type = NOTIFICATION;
        packet->length = sizeof (FrontEndPayload);

        FrontEndPayload *frontEndPayload = new FrontEndPayload;
        strncpy(frontEndPayload->senderUsername, username.c_str(), 100);
        strncpy(frontEndPayload->commandContent, notificationID.c_str(), 100);
        memcpy(packet->_payload, frontEndPayload->toBytes(), BUFFER_SIZE);

        return packet;
    }


    ErrorCodes CommunicationManager::sendNotificationToRMs(string username, string notificationID) {
        Packet *packet = this->createNotificationIDPacket(username, notificationID);
        return sendPacketToRMS(packet);
    }

    ErrorCodes CommunicationManager::sendNotificationToFrontEnds(string username,
                                                                 Notification notification) {

        Packet *packet = this->createNotificationPacket(username, notification);
        return sendPacketToFrontEnds(packet);
    }

    ErrorCodes CommunicationManager::sendPacketToFrontEnds(Packet *packet) {

        vector<FrontEndInfo> frontEnds = GlobalManager::frontEndManager.getFrontEnds();

        for (auto frontEnd : frontEnds) {
            if (send_packet(frontEnd.sendSocket, packet) == SUCCESS) {
                cout << "SUCESSO enviando pacote para front end" << endl;
           } else {
                cout << "ERRO enviando pacote para front end" << endl;
            }
        }

        // assumimos garantia de entrega para front end
        return SUCCESS;
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

        cout << "Criando pacote vazio de " << stringDescribingType(type) << endl;

        Packet package;
        package.type = type;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        package.length = strlen(package._payload);
        return package;
    }

    Packet CommunicationManager::createPacketWithID(int _id, PacketType type) {

        Packet package;
        package.type = type;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        strncpy(package._payload, to_string(_id).c_str(), BUFFER_SIZE);
        package.length = strlen(package._payload);
        return package;
    }

    Packet CommunicationManager::createExitPacket(int idCurrentProcess) {

        cout << "Criando pacote EXIT " << endl;
        return createPacketWithID(idCurrentProcess, EXIT_SERVER);
    }

    Packet CommunicationManager::createCoordinatorPacket(int idCurrentProcess) {

        cout << "Criando pacote COORDINATOR " << endl;
        return createPacketWithID(idCurrentProcess, COORDINATOR);
    }

    Packet CommunicationManager::createHelloPacket(int idCurrentProcess, PacketType typeOfHello) {

        return createPacketWithID(idCurrentProcess, typeOfHello);
    }

    /*
     * Creates exit packet. Sent when server receives a control C.
     * The payload is a buffer with BUFFER_SIZE zeros
     */
    Packet CommunicationManager::createExitPacket() {

        cout << "Criando pacote EXIT" << endl;

        Packet package;
        package.type = EXIT_SERVER;
        package.timestamp = time(NULL);
        bzero(package._payload, BUFFER_SIZE);
        package.length = BUFFER_SIZE;
        return package;
    }

}
