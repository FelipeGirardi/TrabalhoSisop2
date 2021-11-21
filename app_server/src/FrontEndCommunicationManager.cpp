//
// Created by Laura Corssac on 11/13/21.
//

#include "../include/FrontEndCommunicationManager.hpp"
#include "../include/ServerAndFrontEndInfo.h"
#include "../../common/include/PacketType.hpp"
#include "../include/GlobalManager.hpp"
#include "../../common/include/FrontEndPayload.hpp"
#include <netinet/in.h> //sockaddr_in
#include <cstring> //strcpy
#include <sys/socket.h> //connect
#include <netdb.h> //getHostByName
#include <unistd.h> //read

void FrontEndCommunicationManager::setFrontEnds(vector<FrontEndInfo> frontEnds) {
    this->frontEnds = frontEnds;
}


void FrontEndCommunicationManager::sendHelloToFrontEnds() {
    cout << "Iniciando o envio de HELLO para front ends" << endl;

    int numberOfFrontEnds = this->frontEnds.size();

    for (int i = 0; i < numberOfFrontEnds; i++) {
        cout << "Enviando HELLO SEND para front end de IP = " << frontEnds[i].ip << endl;
        ErrorCodes successSend = sendHelloToFrontEnd(frontEnds[i], HELLO_SEND, i);
        if (successSend) {
            cout << "Sucesso enviando HELLO SEND" << endl;
            cout << "Enviando HELLO RECEIVE" << endl;
            ErrorCodes successReceive = sendHelloToFrontEnd(frontEnds[i], HELLO_RECEIVE, i);
            if (successReceive) {
                cout << "Sucesso enviando HELLO RECEIVE" << endl;
            }
        }

    }
}

ErrorCodes FrontEndCommunicationManager::sendHelloToFrontEnd(FrontEndInfo frontEndInfo,
    PacketType type, int idFrontEnd) {

    ServerArguments* _arguments = new ServerArguments;
    _arguments->ip = frontEndInfo.ip;
    _arguments->port = frontEndInfo.port;
    _arguments->typeOfPacket = type;

    pthread_t connect_thread;
    void* threadReturnValue;
    pthread_create(&connect_thread, NULL, &(this->connectToFrontEnd), (void*)_arguments);
    pthread_join(connect_thread, &threadReturnValue);
    int* returnResult = (int*)threadReturnValue;

    if (*returnResult != INVALID_SOCKET) {
        if (_arguments->typeOfPacket == HELLO_SEND) {

            GlobalManager::frontEndManager.setSendSocket(*returnResult, idFrontEnd);
        }
        else if (_arguments->typeOfPacket == HELLO_RECEIVE) {
            GlobalManager::frontEndManager.setReceiveSocket(*returnResult, idFrontEnd);

            cout << "Criando thread de leitura de comandos de front end de ip = " << frontEndInfo.ip << endl;
            int* pointerToSocket = (int*)malloc(sizeof(int));
            *pointerToSocket = *returnResult;
            pthread_t receiving_thread;
            pthread_create(&receiving_thread, NULL, &client_thread_func, (void*)pointerToSocket);

        }
        return SUCCESS;
    }
    else {
        cout << "resultado invalido" << endl;
    }
    return ERROR;

}

// espera ServerArguments como argumento
void* FrontEndCommunicationManager::connectToFrontEnd(void* data) {

    // cria conexao com front end
    struct sockaddr_in serv_addr;
    struct hostent* server;
    int sockfd;
    int timeout_in_seconds = 2;
    int* returnValue = (int*)malloc(sizeof(int));
    ServerArguments* _arguments = (ServerArguments*)data;
    string serverIP = _arguments->ip;
    int port = _arguments->port;
    PacketType packetType = _arguments->typeOfPacket;

    // seta timeout
    struct timeval tv;
    tv.tv_sec = timeout_in_seconds;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    server = gethostbyname(serverIP.c_str());
    if (server == NULL) {
        cout << "ERROR connecting to server. Server is NULL" << endl;
        *returnValue = INVALID_SOCKET;
        return (void*)returnValue;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "ERROR opening socket" << endl;
        *returnValue = INVALID_SOCKET;
        return (void*)returnValue;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = *((struct in_addr*)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ERROR connecting" << endl;
        *returnValue = INVALID_SOCKET;
        return (void*)returnValue;
    }

    Packet* packet = new Packet;
    int idCurrentProcess = GlobalManager::electionManager.getProcessID();
    *packet = GlobalManager::commManager.createHelloPacket(idCurrentProcess, packetType);
    if (GlobalManager::commManager.send_packet(sockfd, packet) == ERROR) {
        *returnValue = INVALID_SOCKET;
        return (void*)returnValue;
    }

    //espera ACK
    Packet* receivedPacket = new Packet;
    int readResult = read(sockfd, receivedPacket, sizeof(Packet));

    // se estorou o tempo -> servidor nao esta ativo
    if (readResult < 0 || receivedPacket == NULL || receivedPacket->type == 0) {
        cout << "Servidor de ip = " << serverIP << " não respondeu no tempo" << endl;
        *returnValue = INVALID_SOCKET;
        return (void*)returnValue;
    }
    // se recebeu algo -> ok
    else {
        *returnValue = sockfd;
    }

    free(receivedPacket);

    return (void*)returnValue;

}

// recebe socket como argumento
void* FrontEndCommunicationManager::client_thread_func(void* data) {
    int readResult;
    char buffer[BUFFER_SIZE];
    int _exit = 0;
    int* socket = (int*)data;
    int commandSocket = *socket;

    while (!_exit) {
        bzero(buffer, BUFFER_SIZE);

        Packet* receivedPacket = new Packet;
        readResult = read(commandSocket, receivedPacket, sizeof(Packet));

        if (readResult < 0 || !(receivedPacket->type == SEND ||
            receivedPacket->type == FOLLOW ||
            receivedPacket->type == EXIT ||
            receivedPacket->type == LOGIN)) {
            free(receivedPacket);
            printf("ERRO lendo do socket de front end. Desconectando.\n");
            break;
        }

        strcpy(buffer, receivedPacket->_payload);

        Packet* responsePacket = new Packet;

        FrontEndPayload* frontEndPayload = FrontEndPayload::fromBytes(receivedPacket->_payload);
        cout << "Payload do pacote recebido:" << endl;
        cout << "sender username = " << frontEndPayload->senderUsername << endl;
        cout << "command content = " << frontEndPayload->commandContent << endl;

        //replicar
        ErrorCodes sendPacketResult = GlobalManager::commManager.sendPacketToRMS(receivedPacket);

        if (sendPacketResult == ERROR) {
            *responsePacket = GlobalManager::commManager.createGenericNackPacket();
        }
        else if (receivedPacket->type == FOLLOW) {
            cout << "Recebeu comando FOLLOW" << endl;

            ErrorCodes followResult = GlobalManager::sessionManager
                .addNewFollowerToUser(frontEndPayload->senderUsername,
                    frontEndPayload->commandContent);

            if (followResult == SUCCESS)
                *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
            else
                *responsePacket = GlobalManager::commManager.createGenericNackPacket();

        }
        else if (receivedPacket->type == SEND) {
            cout << "Recebeu comando SEND" << endl;

            GlobalManager::notifManager.newNotificationSentBy(frontEndPayload->senderUsername,
                frontEndPayload->commandContent);

            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);

        }
        else if (receivedPacket->type == EXIT) {
            cout << "Recebeu comando EXIT" << endl;

            GlobalManager::sessionManager.endSessionWithID(frontEndPayload->commandContent,
                frontEndPayload->senderUsername);

            GlobalManager::sessionManager
                .additionalSessionClosingProcedure(frontEndPayload->senderUsername);

            cout << frontEndPayload->senderUsername << " desconectado" << endl;

            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);

        }
        else if (receivedPacket->type == LOGIN) {
            cout << "Recebeu comando LOGIN" << endl;

            if (GlobalManager::sessionManager.createNewSession(frontEndPayload->senderUsername,
                frontEndPayload->commandContent) == ERROR) {

                *responsePacket = GlobalManager::commManager.createGenericNackPacket();
            }
            else {
                *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);

                // Verifica se é a primeira sessão criada
                GlobalManager::sessionManager
                    .additionalSessionOpeningProcedure(frontEndPayload->senderUsername);

            }

        }

        cout << "Enviando pacote ACK/NACK recebimento de comando **" << endl;
        if (GlobalManager::commManager.send_packet(commandSocket, responsePacket) == ERROR) {
            cout << "Não foi possivel enviar ACK/NACK" << endl;
        }
        else {
            cout << "ACK/NACK enviado com sucesso" << endl;
        }
        free(receivedPacket);
        free(responsePacket);

    }

    return 0;
}

void FrontEndCommunicationManager::setSendSocket(int socket, int _id) {
    this->frontEnds[_id].sendSocket = socket;
}

void FrontEndCommunicationManager::setReceiveSocket(int socket, int _id) {
    this->frontEnds[_id].receiveSocket = socket;
}

vector<FrontEndInfo> FrontEndCommunicationManager::getFrontEnds() {
    return this->frontEnds;
}
