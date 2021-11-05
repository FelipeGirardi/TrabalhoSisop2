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

ElectionManager::ElectionManager() {

}

void ElectionManager::startElection() {

    int numberOfServers = this->servers.size();
    pthread_t threadIDs[numberOfServers];
    bool hasReceivedSuccess = false;

    // para cada servidor com ID maior
    for (int i= this->currentServerID+1; i < numberOfServers; i++) {
        int sendSocket = this->servers[i].sendSocket;
        ReceiveThreadArguments *_arguments = new ReceiveThreadArguments;
        _arguments->socket = sendSocket;

        pthread_t send_thread;
        pthread_create(&send_thread, NULL, &send_election_message, (void *) _arguments);
        threadIDs[i] = send_thread;
    }

    for (int i= this->currentServerID+1; i < numberOfServers; i++) {

        void *threadReturnValue;
        pthread_join(threadIDs[i], &threadReturnValue);

        ErrorCodes *returnResult = (ErrorCodes *) threadReturnValue;
        hasReceivedSuccess = returnResult == SUCCESS;

    }

    if (hasReceivedSuccess) {
        cout << "ESPERA O COORDINATOR" << endl;
    } else {
        cout << "SE ASSUME LIDER" << endl;
    }

}

void* ElectionManager::receive_messages(void *data) {

    cout << "Iniciando leitura de pacotes ELECTION" << endl;

    ReceiveThreadArguments *receivedArguments = (ReceiveThreadArguments *)data;
    int socket = receivedArguments->socket;

    bool shouldStartElection = false
    Packet *responsePacket = new Packet;
    Packet *receivedPacket = new Packet;

    while (true) {

        responsePacket = new Packet;
        receivedPacket = new Packet;
        shouldStartElection = false

        int readResult = read(socket, receivedPacket, sizeof(Packet));
        if (readResult < 0 || (receivedPacket->type != ELECTION)) {
            cout << "ERRO lendo do socket recebimento ELECTION. Fechando." << endl;
            *responsePacket = GlobalManager::commManager.createGenericNackPacket();
        } else {
            cout << "Recebeu ELECTION com sucesso." << endl;
            *responsePacket = GlobalManager::commManager.createEmptyPacket(ANSWER);
            shouldStartElection = true;
        }

        cout << "Enviando pacote ANSWER/NACK recebimento" << endl;
        if (GlobalManager::commManager.send_packet(socket, responsePacket) == ERROR) {
            cout << "ERRO enviando ANSWER/NACK" << endl;
            shouldStartElection = false;
        } else {
            cout << "ANSWER/NACK enviado com sucesso" << endl;
        }

        if (shouldStartElection) {
            cout << "Starting new election" << endl;
            GlobalManager::electionManager.startElection();
        }

    }
    free(receivedPacket);
    free(responsePacket);

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

    // se recebeu algo -> ok
    if (readResult < 0 || receivedPacket == NULL || receivedPacket->type != ANSWER) {
        returnValue = ERROR;
    }

    // se estorou o tempo -> primario caiu
    else {
        cout << "Recebeu answer com sucesso" << endl;
        returnValue = SUCCESS;
    }

    delete receivedPacket;
    delete sendPacket;
    return returnValue;

}