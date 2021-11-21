#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "../include/ProfileSessionManager.hpp"
#include "../include/UserInformation.hpp"
#include "../include/FileManager.hpp"
#include "../include/CommunicationManager.hpp"
#include "../include/Session.hpp"
#include "../include/GlobalManager.hpp"
#include "../include/utils/ErrorCodes.hpp"
#include "../../common/include/Packet.hpp"
#include "../../common/include/SessionAuth.hpp"
#include "../include/ServerAndFrontEndInfo.h"
#include "../../common/include/FrontEndPayload.hpp"

using namespace userInformation;
using namespace profileSessionManager;
using namespace communicationManager;
using namespace Common;

unordered_map<string, UserInformation> users;
unordered_map<string,Notification> notifications;
FileManager fileManager;
ProfileSessionManager sessionManager;
GlobalManager globalManager;
NotificationManager notificationManager;
CommunicationManager comunicationManager;
ElectionManager electionManager;
bool shouldEnd = false;

//TODO: mudar de lugar
typedef struct AuthResult {
    ErrorCodes result;
    SessionAuth *sessionAuth;
    bool shouldCreateListenThread;
}AuthResult;

// Declaracao funcoes auxiliares
void closeAppHandler(int n_signal);
void *connectToServer(void *data);

// Declaracao de funcoes de threads
void *auth_client_func(void *data);
void *send_keep_alive_thread_func(void *data);
void *receive_server_events_thread_func(void *data);
void sendHelloToServers(vector<ServerInfo> servers);
ErrorCodes sendHelloToServer(ServerArguments _arguments, int _id);

pthread_t keep_alive_thread;
int port;

int main(int argc, char* argv[])
{

    if (argc < 3) {
        cout << "" << endl;
        cout << "usage: ./app_server [server-ID] [server-port] " << endl;
        return 0;
    }
    int serverID = stoi(argv[1]);
    int serverPort = stoi(argv[2]);
    port = serverPort;

    //instanciação dos managers
    GlobalManager::sessionManager = sessionManager;
    GlobalManager::notifManager = notificationManager;
    GlobalManager::commManager = comunicationManager;
    GlobalManager::electionManager = electionManager;

    // pega do arquivo instancias do servidor (só com ID)
    vector<ServerInfo> servers = fileManager.getServersFromFile();

    //seta servidores no election manager
    GlobalManager::electionManager.setServers(servers);

    // seta id deste servidor
    GlobalManager::electionManager.setCurrentServerID(serverID);

    // Pega IP servidor
    string serverIP = GlobalManager::electionManager.getIPfromID(serverID);

    // pega do arquivo instancias do front end
    vector<FrontEndInfo> frontEnds = fileManager.getFrontEndsFromFile();
    GlobalManager::frontEndManager.setFrontEnds(frontEnds);

	int sockfd, option = 1;
	struct sockaddr_in serv_addr;

    setbuf(stdout, NULL);  // zera buffer do stdout

    cout << "Criando socket servidor\n";
    // cria socket TCP verificando erro
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "ERRO abrindo socket **\n";
        return 0;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // seta dados do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);
    serv_addr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    bzero(&(serv_addr.sin_zero), 8);

    // faz o bind
    cout << "Fazendo bind IP e porta\n";
    if (::bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ERRO no bind **\n";
        return 0;
    }

    // seta signal de fechar app do servidor
    signal(SIGINT, closeAppHandler);

    // ignora o signal do write em socket fechado
    signal (SIGPIPE, SIG_IGN);

    // loop do listen de conexões
    if (listen(sockfd, 5) == 0) {
        int client_sockfd, notif_sockfd; // usar notif_sockfd se precisar

        // manda HELLO para todos os servidores
        sendHelloToServers(servers);

        while(true) {
            // aceita conexão do cliente
            struct sockaddr_in client_address;
            socklen_t client_length = sizeof(struct sockaddr_in);
            cout << "Esperando conexão \n";
            if ((client_sockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_length)) == -1) {
                cout << "ERRO aceitando conexão canal de comandos\n";
                continue;
            }

            // Creates thread for receiving username
            pthread_t auth_thread;
            cout << "Criando thread autenticação" << endl;
            int *pointerToSocket = (int*) malloc(sizeof (int));
            *pointerToSocket = client_sockfd;
            pthread_create(&auth_thread, NULL, &auth_client_func, (void *) pointerToSocket);

            void *resultOfAuthentication;
            pthread_join(auth_thread, &resultOfAuthentication);
            AuthResult *myResult = (AuthResult *)resultOfAuthentication;

            if (myResult->result != SUCCESS) { continue; }

            // conexão de um outro RM
            if (myResult->shouldCreateListenThread){
                cout << "Criando thread de recebimento de mensagens de servidores" << endl;
                pthread_t keep_alive_thread;
                pthread_create(&keep_alive_thread, NULL, &receive_server_events_thread_func, (void *) pointerToSocket);
            }

        }
    }

    if (close(sockfd) < 0) {
        cout << "ERRO fechando o socket do servidor" << endl;
    } else {
        cout << "Sucesso fechando o socket do servidor" << endl;
    }

    return 0;
}

ErrorCodes sendHelloToServer(ServerArguments _arguments, int _id) {

    ServerArguments *_argumentsPointer = new ServerArguments;
    *_argumentsPointer = _arguments;

    pthread_t connect_thread;
    void *threadReturnValue;
    pthread_create(&connect_thread, NULL, &connectToServer, (void *) _argumentsPointer);
    pthread_join(connect_thread, &threadReturnValue);
    int *returnResult = (int *) threadReturnValue;

    if (*returnResult != INVALID_SOCKET) {
        if (_arguments.typeOfPacket == HELLO_SEND) {

            GlobalManager::electionManager.setSendSocket(*returnResult, _id);

        } else if (_arguments.typeOfPacket == HELLO_RECEIVE) {
            GlobalManager::electionManager.setReceiveSocket(*returnResult, _id);

            // cria thread recebimento
            cout << "Criando thread recebimento de msg outros servidores" << endl;
            int *pointerToSocket = (int*) malloc(sizeof (int));
            *pointerToSocket = *returnResult;
            pthread_t receiving_thread;
            pthread_create(&receiving_thread, NULL, &receive_server_events_thread_func, (void *) pointerToSocket);

        }
        return SUCCESS;
    } else {
        cout << "resultado invalido" << endl;
    }
    return ERROR;

}

void sendHelloToServers(vector<ServerInfo> servers) {
    cout << "Iniciando o envio de HELLO para servidores" << endl;

    int numberOfServers = GlobalManager::electionManager.getNumberOfServers();
    int validResponseCounter = 0;

    for (int i = 0; i < servers.size(); i++) {
        if (i == GlobalManager::electionManager.getProcessID()) { continue; }

        string _ip = GlobalManager::electionManager.getIPfromID(i);

        ServerArguments _arguments = { .ip=_ip, .port=port, .typeOfPacket= HELLO_SEND,};
        cout << "Enviando HELLO SEND para servidor de ip = " << _ip << endl;
        ErrorCodes successSend = sendHelloToServer(_arguments, i);
        if (successSend) {
            cout << "Sucesso enviando HELLO SEND" << endl;
            ServerArguments _arguments2 = { .ip=_ip, .port=port, .typeOfPacket= HELLO_RECEIVE,};
            cout << "Enviando HELLO RECEIVE para servidor de ip = " << _ip << endl;
            ErrorCodes successReceive = sendHelloToServer(_arguments2, i);
            if (successReceive) {
                cout << "Sucesso enviando HELLO RECEIVE" << endl;
                validResponseCounter += 1;
            }
        }

    }
    //se assume lider
    if (validResponseCounter == 0) {
        cout << "Assumindo liderança - Primeiro Primário" << endl;
        GlobalManager::electionManager.setNewCoordinatorIDToItself();
        cout << "Inicia envio de Hello para Front ends" << endl;
        GlobalManager::frontEndManager.sendHelloToFrontEnds();
    } else  {
        cout << "Já há um líder." << endl;
    }

}

void *connectToServer(void *data) {

    // cria conexao com outro servidor
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;
    int timeout_in_seconds = 2;
    int *returnValue = (int *) malloc(sizeof (int));
    ServerArguments *_arguments = (ServerArguments *) data;
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
        cout << "ERRO conectando ao servidor. Server não existe" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "ERRO abrindo socket" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        cout << "ERRO conectando" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }

    Packet *packet = new Packet;
    int idCurrentProcess = GlobalManager::electionManager.getProcessID();
    *packet = GlobalManager::commManager.createHelloPacket(idCurrentProcess, packetType);
    if (GlobalManager::commManager.send_packet(sockfd, packet) == ERROR) {
        cout << "ERRO enviando HELLO number = " << packetType << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    } else {
        cout << "Sucesso enviando HELLO number = " << packetType << endl;
    };

    //espera ACK
    Packet *receivedPacket = new Packet;
    int readResult = read(sockfd, receivedPacket, sizeof(Packet));

    // se estorou o tempo -> servidor nao esta ativo
    if (readResult < 0 || receivedPacket == NULL || receivedPacket->type == 0) {
        cout << "Servidor não respondeu no tempo" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }
    // se recebeu algo -> ok
    else {
        *returnValue = sockfd;
    }

    free(receivedPacket);

    return (void *) returnValue;

}

void *send_keep_alive_thread_func(void *data) {

    cout << "Iniciando envio de pacotes KEEP ALIVE." << endl;

    int *socket = (int*) data;
    int timeBetweenMessages = 10;
    int timeout_in_seconds = 2;

    // seta timeout
    struct timeval tv;
    tv.tv_sec = timeout_in_seconds;
    tv.tv_usec = 0;
    setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    while (true) {
        // espera tempo entre mensagens keep alive
        sleep(timeBetweenMessages);

        // cria pacote KEEP ALIVE
        Packet *packet = new Packet;
        *packet = GlobalManager::commManager.createEmptyPacket(KEEP_ALIVE);

        // envia
        cout << "Enviando pacote KEEP ALIVE" << endl;
        if (GlobalManager::commManager.send_packet(*socket, packet) == ERROR) {
            cout << "Não foi possivel enviar KEEP ALIVE" <<endl;
        } else {
            cout << "KEEP ALIVE enviado com sucesso" << endl;
        }

        Packet *receivedPacket = new Packet;
        int readResult = read(*socket, receivedPacket, sizeof(Packet));

        // se estorou o tempo -> primario caiu
        if (readResult < 0 || receivedPacket == NULL || receivedPacket->type == 0) {
            cout << "Primário caiu" << endl;
            GlobalManager::electionManager.startElection();
            return 0;
        }
        // se recebeu algo -> ok
        else {
            cout << "Primário tá vivo" << endl;
        }

    }

}

// recebe mensagens EXIT_SERVER, COORDINATOR, ELECTION e KEEP ALIVE,
// LOGIN, EXIT, FOLLOW e SEND
void *receive_server_events_thread_func(void *data) {

    cout << "Iniciando leitura de pacotes de outros servidores." << endl;

    int *receivedSocket = (int*) data;
    Packet *responsePacket = new Packet;
    Packet *receivedPacket = new Packet;
    bool shouldStartElection = false;

    while (true) {

        responsePacket = new Packet;
        receivedPacket = new Packet;

        int readResult = read(*receivedSocket, receivedPacket, sizeof(Packet));

        if (readResult < 0 ||
        ((receivedPacket->type != KEEP_ALIVE) &&
        (receivedPacket->type != COORDINATOR) &&
        (receivedPacket->type != ELECTION) &&
        (receivedPacket->type != EXIT) &&
        (receivedPacket->type != LOGIN) &&
        (receivedPacket->type != FOLLOW) &&
        (receivedPacket->type != SEND) &&
        (receivedPacket->type != EXIT_SERVER) &&
        (receivedPacket->type != NOTIFICATION)
        )) {
            cout << "ERRO lendo do socket recebimento mensagens de outros RM. Fechando." << endl;
            *responsePacket = GlobalManager::commManager.createGenericNackPacket();
        } else if (receivedPacket->type == KEEP_ALIVE) {
            cout << "Recebeu mensagem KEEP_ALIVE com sucesso." << endl;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(KEEP_ALIVE);
        } else if (receivedPacket->type == COORDINATOR) {
            cout << "Recebeu mensagem COORDINATOR com sucesso." << endl;
            int coordinatorID = atoi(receivedPacket->_payload);
            cout << "ID novo coordinator = " << coordinatorID << endl;
            GlobalManager::electionManager.setNewCoordinatorID(coordinatorID);

            cout << "Cancela envio de keep alive para antigo coordinator" << endl;
            pthread_cancel(keep_alive_thread);
            cout << "Inicia envio de keep alive para novo coordinator" << endl;
            pthread_t newKeepAliveID;
            keep_alive_thread = newKeepAliveID;
            int *pointerToSocket = (int*) malloc(sizeof (int));
            *pointerToSocket = GlobalManager::electionManager.getCurrentCoordinatorSendSocket();

            pthread_create(&keep_alive_thread, NULL, &send_keep_alive_thread_func, (void *) pointerToSocket);

            // nao há ack/answer para mensagens coordinator
            continue;

        } else if (receivedPacket->type == ELECTION) {
            cout << "Recebeu ELECTION" << endl;
            *responsePacket = GlobalManager::commManager.createEmptyPacket(ANSWER);
            shouldStartElection = true;
        } else if (receivedPacket->type == EXIT_SERVER) {
            cout << "Recebeu EXIT SERVER" << endl;
            int _id = atoi(receivedPacket->_payload);
            cout << "id do servidor remetente = " << _id << endl;
            GlobalManager::electionManager.setSendSocket(INVALID_SOCKET, _id);

            // nao há ack/answer para mensagens EXIT
            free(receivedPacket);
            free(responsePacket);
            return 0;

        } else if (receivedPacket->type == LOGIN) {

            cout << "Recebeu comando LOGIN" << endl;
            FrontEndPayload *frontEndPayload = FrontEndPayload::fromBytes(receivedPacket->_payload);

            if (GlobalManager::sessionManager.createNewSession(frontEndPayload->senderUsername,
                                                               frontEndPayload->commandContent) == ERROR) {

                *responsePacket = GlobalManager::commManager.createGenericNackPacket();
            } else {
                *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
            }

        } else if (receivedPacket->type == SEND) {

            cout << "Recebeu comando SEND" << endl;
            FrontEndPayload *frontEndPayload = FrontEndPayload::fromBytes(receivedPacket->_payload);

            GlobalManager::notifManager.newNotificationSentBy(frontEndPayload->senderUsername,
                                                              frontEndPayload->commandContent);

            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);

        } else if (receivedPacket->type == FOLLOW) {

            cout << "Recebeu comando FOLLOW" << endl;
            FrontEndPayload *frontEndPayload = FrontEndPayload::fromBytes(receivedPacket->_payload);
            ErrorCodes followResult = GlobalManager::sessionManager
                    .addNewFollowerToUser(frontEndPayload->senderUsername,
                                          frontEndPayload->commandContent);

            if (followResult == SUCCESS)
                *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
            else
                *responsePacket = GlobalManager::commManager.createGenericNackPacket();

        } else if (receivedPacket->type == EXIT) {
            cout << "Recebeu comando EXIT de front end" << endl;

            FrontEndPayload *frontEndPayload = FrontEndPayload::fromBytes(receivedPacket->_payload);
            GlobalManager::sessionManager.endSessionWithID(frontEndPayload->commandContent,
                                                          frontEndPayload->senderUsername);

        } else if (receivedPacket->type == NOTIFICATION) {
            cout << "Recebeu pedido de deleção de notificação" << endl;
            FrontEndPayload *frontEndPayload = FrontEndPayload::fromBytes(receivedPacket->_payload);
            GlobalManager::sessionManager.deleteNotificationFromUser(frontEndPayload->senderUsername,
                                                                     frontEndPayload->commandContent);
            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
        }

        cout << "Enviando pacote ACK/NACK para outro RM" << endl;
        if (GlobalManager::commManager.send_packet(*receivedSocket, responsePacket) == ERROR) {
            cout << "ERRO enviando ACK/NACK" << endl;
            shouldStartElection = false;
        } else {
            cout << "ACK/NACK enviado com sucesso" << endl;
        }

        if (shouldStartElection) {
            cout << "Iniciando eleicao" << endl;
            GlobalManager::electionManager.startElection();
        }

    }
    free(receivedPacket);
    free(responsePacket);

}

/*
 * Funcao executada por uma thread para pegar o username do usuário
 * Retorno = estrutura AuthResult
 */

void *auth_client_func(void *data) {

    int *receivedSocket = (int*) data;
    int client_socket = *receivedSocket;
    int shouldSendCoordinator = false;
    Packet *receivedPacket = new Packet;
    AuthResult *finalResult = new AuthResult;
    Packet *responsePacket = new Packet;

    int readResult = read(client_socket, receivedPacket, sizeof (Packet));
    if (readResult < 0 || (receivedPacket->type != EXIT
    && receivedPacket->type != HELLO_SEND
    && receivedPacket->type != HELLO_RECEIVE)) {
        cout <<"ERRO lendo do socket. Fechando." << endl;
        finalResult->result = ERROR;
        finalResult->sessionAuth = NULL;
        finalResult->shouldCreateListenThread = false;
        *responsePacket = GlobalManager::commManager.createGenericNackPacket();
    } else if (receivedPacket->type == EXIT) {
        cout <<"Sucesso recebendo EXIT. Fechando." << endl;
        finalResult->result = ERROR;
        finalResult->sessionAuth = NULL;
        finalResult->shouldCreateListenThread = false;
        *responsePacket = GlobalManager::commManager.createAckPacketForType(EXIT);
    } else if (receivedPacket->type == HELLO_SEND) {
        cout << "HELLO SEND recebido com sucesso" << endl;
        *responsePacket = GlobalManager::commManager.createAckPacketForType(HELLO_SEND);
        int newProcessID = atoi(receivedPacket->_payload);
        GlobalManager::electionManager.setReceiveSocket(client_socket, newProcessID);
        finalResult->result = SUCCESS;
        finalResult->sessionAuth = NULL;
        finalResult->shouldCreateListenThread = true;
    } else if (receivedPacket->type == HELLO_RECEIVE) {
        cout << "HELLO RECEIVE recebido com sucesso" << endl;
        int newProcessID = atoi(receivedPacket->_payload);
        GlobalManager::electionManager.setSendSocket(client_socket, newProcessID);

        finalResult->result = SUCCESS;
        finalResult->sessionAuth = NULL;
        finalResult->shouldCreateListenThread = false;
        shouldSendCoordinator = GlobalManager::electionManager.isCurrentProcessCoordinator();

        *responsePacket = GlobalManager::commManager.createAckPacketForType(HELLO_RECEIVE);

    }

    cout << "Enviando pacote ACK/NACK recebimento de comando" << endl;
    if (GlobalManager::commManager.send_packet(client_socket, responsePacket) == ERROR) {
        cout << "ERRO enviando ACK/NACK" <<endl;
    } else {
        cout << "ACK/NACK enviado com sucesso" << endl;
    }

    if (shouldSendCoordinator) {
        cout << "Enviando pacote coordinator para nova conexão" << endl;
        int _id = GlobalManager::electionManager.getProcessID();
        *responsePacket =  GlobalManager::commManager.createCoordinatorPacket(_id);
        if (GlobalManager::commManager.send_packet(client_socket, responsePacket) == ERROR) {
            cout << "ERRO enviando COORDINATOR" <<endl;
        } else {
            cout << "COORDINATOR enviado com sucesso" << endl;
        }
    }

    free(receivedPacket);
    free(responsePacket);

    if (finalResult->result == ERROR && finalResult->sessionAuth != NULL) {
        GlobalManager::sessionManager.endSessionWithID(finalResult->sessionAuth->getUuid(),
                                                       finalResult->sessionAuth->getProfileId());
    }

    return (void *) finalResult;

}

void closeAppHandler(int n_signal) {
    signal(n_signal, SIG_IGN);

//    Packet exitPacket = GlobalManager::commManager.createExitPacket();
//    Packet *packetPointer = (Packet *) malloc(sizeof (Packet));
//    *packetPointer = exitPacket;

// TODO: ENVIAR EXIT OU EXIT/SERVER PARA FRONT ENDS

//    list<Session> allSessions = GlobalManager::sessionManager.getAllSessions();
//    if (GlobalManager::commManager.sendPacketToSessions(allSessions, packetPointer) == SUCCESS) {
//        cout << "Sucesso enviando EXIT para todas as sessões" << endl;
//    } else {
//        cout << "ERRO enviando EXIT para todas as sessões" << endl;
//    }

    GlobalManager::electionManager.sendExitToAllOtherServers();

    GlobalManager::sessionManager.endAllSessions();
    GlobalManager::electionManager.printItself();

    //free(packetPointer);

    exit(0);
}
