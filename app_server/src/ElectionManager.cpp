//
// Created by Laura Corssac on 10/31/21.
//

#include "../include/ElectionManager.hpp"
#include "../include/utils/ErrorCodes.hpp"
#include "../include/CommunicationManager.hpp"
#include "../include/GlobalManager.hpp"
#include "../../common/include/Packet.hpp"
#include "../../common/include/PacketType.hpp"
#include <sys/socket.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

ElectionManager::ElectionManager() {

}

void ElectionManager::startElection() {

    cout << "Starting a new election" << endl;

    int numberOfServers = this->servers.size();

    bool hasReceivedSuccess = false;

    // para cada servidor com ID maior
    for (int i= this->currentServerID+1; i < numberOfServers; i++) {

        cout << "enviando ELECTION para id = " << i << endl;

        int sendSocket = this->servers[i].sendSocket;
        ReceiveThreadArguments *_arguments = new ReceiveThreadArguments;
        _arguments->socket = sendSocket;

        pthread_t send_thread;
        pthread_create(&send_thread, NULL, &send_election_message, (void *) _arguments);

        void *threadReturnValue;
        pthread_join(send_thread, &threadReturnValue);
        ErrorCodes *returnResult = (ErrorCodes *) threadReturnValue;
        hasReceivedSuccess = (*returnResult == SUCCESS);

    }

    if (hasReceivedSuccess) {
        cout << "ESPERA O COORDINATOR" << endl;
    } else {
        GlobalManager::electionManager.assumeCoordination();
    }

}

void ElectionManager::sendCoordinatorPacket(int sendSocket) {

    Packet *packet = new Packet;
    *packet = GlobalManager::commManager.createCoordinatorPacket(this->currentServerID);

    if (GlobalManager::commManager.send_packet(sendSocket, packet) == ERROR) {
        cout << "ERRO enviando COORDINATOR" << endl;
    } else {
        cout << "COORDINATOR enviado com sucesso" << endl;
    }
    free(packet);

}

// envia mensagem de coordinator para todos os processos
void ElectionManager::assumeCoordination() {

    setNewCoordinatorIDToItself();
    int numberOfServers = this->servers.size();

    for (int i= 0; i < numberOfServers; i++) {
        if (currentServerID == i) { continue; }
        int sendSocket = this->servers[i].sendSocket;
        cout << "Enviando pacote COORDINATOR para id = " << i << endl;
        sendCoordinatorPacket(sendSocket);
    }
}

void* ElectionManager::send_election_message(void *data) {

    ErrorCodes returnValue;
    int *socket = (int*) data;
    Packet *sendPacket = new Packet;
    *sendPacket = GlobalManager::commManager.createEmptyPacket(ELECTION);
    int timeout_in_seconds = 2; //TODO: tirar daqui e criar constante

    // seta timeout
    struct timeval tv;
    tv.tv_sec = timeout_in_seconds;
    tv.tv_usec = 0;
    setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    // envia
    cout << "Enviando pacote ELECTION" << endl;
    if (GlobalManager::commManager.send_packet(*socket, sendPacket) == ERROR) {
        cout << "Não foi possivel enviar ELECTION" <<endl;
    } else {
        cout << "ELECTION enviado com sucesso" << endl;
    }

    //espera recebimento answer
    Packet *receivedPacket = new Packet;
    int readResult = read(*socket, receivedPacket, sizeof(Packet));

    // se estorou o tempo -> primario caiu
    if (readResult < 0 || receivedPacket == NULL || receivedPacket->type != ANSWER) {
        cout << "ERROR recebendo answer" << endl;
        returnValue = ERROR;
    }
    // se recebeu algo -> ok
    else {
        cout << "Recebeu answer com sucesso" << endl;
        returnValue = SUCCESS;
    }

    delete receivedPacket;
    delete sendPacket;

    return (void *) returnValue;

}

void ElectionManager::setNewCoordinatorIDToItself() {
    this->setNewCoordinatorID(this->currentServerID);
}

void ElectionManager::setNewCoordinatorID(int newCoordinatorID) {
    this->currentCoordinatorID = newCoordinatorID;
}

int ElectionManager::getCoordinatorID() {
    return this->currentCoordinatorID;
}

int ElectionManager::getProcessID() {
    return this->currentServerID;
}

int ElectionManager::getCurrentCoordinatorSendSocket() {
    return this->servers[this->currentCoordinatorID].sendSocket;
}

void ElectionManager::setSendSocket(int sendSocket, int serverID) {
    this->servers[serverID].sendSocket = sendSocket;
}

void ElectionManager::setReceiveSocket(int receiveSocket, int serverID) {
    this->servers[serverID].receiveSocket = receiveSocket;
}
string ElectionManager::getIPfromID(int serverID) {
    return this->servers[serverID].ip;
}
bool ElectionManager::isCurrentProcessCoordinator() {
    return this->currentServerID == this->currentCoordinatorID;
}
bool ElectionManager::hasValidSendSocketForServer(int serverID) {
    return this->servers[serverID].sendSocket != INVALID_SOCKET;
}
void ElectionManager::setCurrentServerID(int serverID) {
    this->currentServerID = serverID;
}
void ElectionManager::setServers(vector<ServerInfo> servers) {
    this->servers = servers;
}
int ElectionManager::getNumberOfServers() {
    return this->servers.size();
}