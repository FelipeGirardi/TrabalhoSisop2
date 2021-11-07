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
}AuthResult;

typedef struct {
    string ip;
    int port;
} ServerIpAndPort;

// Declaracao funcoes auxiliares
void closeAppHandler(int n_signal);
void *connectToServer(void *data);
int connectToServer(ServerIpAndPort _arguments);

// Declaracao de funcoes de threads
void *auth_client_func(void *data);
void *client_thread_func(void *data);
void *send_keep_alive_thread_func(void *data);
void *receive_server_events_thread_func(void *data);
void sendHelloToServers(vector<ServerInfo> servers);

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

    // pega de um arquivo instancias do servidor (só com ID)
    vector<ServerInfo> servers = fileManager.getServersFromFile();

    //seta servidores no election manager
    GlobalManager::electionManager.setServers(servers);

    // seta id deste servidor
    GlobalManager::electionManager.setCurrentServerID(serverID);

    // Pega IP servidor
    string serverIP = GlobalManager::electionManager.getIPfromID(serverID);
    cout << "ip deste servidor = " << serverIP;
    cout << "size = " << serverIP.size();

    // carrega estruturas de dados do arquivo (usuários + notificações)
    users = fileManager.getUsersFromFile();
    notifications = fileManager.getNotificationsFromFile();
    GlobalManager::sessionManager.setUsers(users);
    GlobalManager::notifManager.setNotifications(notifications);

    GlobalManager::printItself();

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
            cout << "novo socket " << client_sockfd << endl;

            // Creates thread for receiving username
            pthread_t auth_thread;
            cout << "Criando thread autenticação" << endl;
            int *pointerToSocket = (int*) malloc(sizeof (int));
            *pointerToSocket = client_sockfd;
            pthread_create(&auth_thread, NULL, &auth_client_func, (void *) pointerToSocket);

            void *resultOfAuthentication;
            pthread_join(auth_thread, &resultOfAuthentication);
            cout << "voltou da thread de auth" << endl;
            AuthResult *myResult = (AuthResult *)resultOfAuthentication;

            if (myResult->result != SUCCESS) { continue; }

            // conexão de um usuário
            if (myResult->sessionAuth != NULL) {
                if (myResult->sessionAuth->getSocketType() == COMMAND_SOCKET) {
                    pthread_t client_thread;
                    cout << "Criando thread de leitura de comandos de usuarios" << endl;
                    SessionAuth *pointerToSessionAuth = myResult->sessionAuth;
                    pthread_create(&client_thread, NULL, &client_thread_func, (void *) pointerToSessionAuth);
                } else {
                    string username = myResult->sessionAuth->getProfileId();
                    GlobalManager::sessionManager.users[username]
                    .startListeningForNotifications();

                }
            }
            // conexão de um outro RM
            else {
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

void sendHelloToServers(vector<ServerInfo> servers) {
    cout << "Iniciando o envio de HELLO" << endl;

    int numberOfServers = GlobalManager::electionManager.getNumberOfServers();
    int validResponseCounter = 0;

    for (int i = 0; i < servers.size(); i++) {
        if (i == GlobalManager::electionManager.getProcessID()) { continue; }

        ServerIpAndPort *_arguments = new ServerIpAndPort;
        _arguments->ip = servers[i].ip;
        _arguments->port = port;

        pthread_t connect_thread;
        void *threadReturnValue;
        pthread_create(&connect_thread, NULL, &connectToServer, (void *) _arguments);
        pthread_join(connect_thread, &threadReturnValue);
        int *returnResult = (int *) threadReturnValue;

        if (*returnResult != INVALID_SOCKET) {
            cout << "resultado valido" << endl;
            GlobalManager::electionManager.setSendSocket(*returnResult, i);
            validResponseCounter += 1;
        } else {
            cout << "resultado invalido" << endl;
        }

    }

    //se assume lider
    if (validResponseCounter == 0) {
        cout << "Assumindo liderança" << endl;
        GlobalManager::electionManager.setNewCoordinatorIDToItself();
    } else  {
        cout << "Já há um líder." << endl;
    }

}

void *connectToServer(void *data) {

    cout << "iniciando connect to server" << endl;
    // cria conexao com outro servidor
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;
    int timeout_in_seconds = 2;
    int *returnValue = (int *) malloc(sizeof (int));
    ServerIpAndPort *_arguments = (ServerIpAndPort *) data;
    string serverIP = _arguments->ip;
    int port = _arguments->port;

    cout << "Connecting to server of ip = " << serverIP << endl;

    // seta timeout
    struct timeval tv;
    tv.tv_sec = timeout_in_seconds;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    server = gethostbyname(serverIP.c_str());
    if (server == NULL) {
        cout << "ERROR connecting to server. Server is NULL" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "ERROR opening socket" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        cout << "ERROR connecting" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    }

    Packet *packet = new Packet;
    int idCurrentProcess = GlobalManager::electionManager.getProcessID();
    *packet = GlobalManager::commManager.createHelloPacket(idCurrentProcess);
    if (GlobalManager::commManager.send_packet(sockfd, packet) == ERROR) {
        cout << "ERROR enviando HELLO SERVER" << endl;
        *returnValue = INVALID_SOCKET;
        return (void *) returnValue;
    } else {
        cout << "Sucesso enviando HELLO SERVER" << endl;
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
        cout << "Servidor respondeu no tempo" << endl;
        *returnValue = sockfd;
    }

    free(receivedPacket);

    return (void *) returnValue;

}

int connectToServer(ServerIpAndPort _arguments) {

    // cria conexao com outro servidor
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;
    int timeout_in_seconds = 2;
    int returnValue;
    string serverIP = _arguments.ip;
    int port = _arguments.port;

    cout << "Connecting to server of ip = " << serverIP << endl;

    // seta timeout
    struct timeval tv;
    tv.tv_sec = timeout_in_seconds;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    server = gethostbyname(serverIP.c_str());
    if (server == NULL) {
        cout << "ERROR connecting to server" << endl;
        return INVALID_SOCKET;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "ERROR opening socket" << endl;
        return INVALID_SOCKET;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        cout << "ERROR connecting" << endl;
        return INVALID_SOCKET;
    }

    Packet *packet = new Packet;
    int idCurrentProcess = GlobalManager::electionManager.getProcessID();
    *packet = GlobalManager::commManager.createHelloPacket(idCurrentProcess);
    if (GlobalManager::commManager.send_packet(sockfd, packet) == ERROR) {
        cout << "ERROR enviando HELLO SERVER" << endl;
        return INVALID_SOCKET;
    } else {
        cout << "Sucesso enviando HELLO SERVER" << endl;
    };

    //espera ACK
    Packet *receivedPacket = new Packet;
    int readResult = read(sockfd, receivedPacket, sizeof(Packet));

    // se estorou o tempo -> servidor nao esta ativo
    if (readResult < 0 || receivedPacket == NULL || receivedPacket->type == 0) {
        cout << "Servidor não respondeu no tempo" << endl;
        return INVALID_SOCKET;
    }
        // se recebeu algo -> ok
    else {
        cout << "Servidor respondeu no tempo" << endl;
        returnValue = sockfd;
    }

    free(receivedPacket);

    return returnValue;

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

// recebe mensagens EXIT, COORDINATOR, ELECTION e KEEP ALIVE
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
        (receivedPacket->type != EXIT)
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
            free(receivedPacket);
            free(responsePacket);
            return 0;

        } else if (receivedPacket->type == ELECTION) {
            cout << "Recebeu ELECTION com sucesso." << endl;
            *responsePacket = GlobalManager::commManager.createEmptyPacket(ANSWER);
            shouldStartElection = true;
        } else if (receivedPacket->type == EXIT) {
            cout << "Recebeu EXIT com sucesso." << endl;
            int _id = atoi(receivedPacket->_payload);
            cout << "id do processo remetente = " << _id << endl;
            GlobalManager::electionManager.setSendSocket(INVALID_SOCKET, _id);

            // nao há ack/answer para mensagens EXIT
            free(receivedPacket);
            free(responsePacket);
            return 0;

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
    cout << "Iniciando authenticação do socket = " << client_socket << endl;
    Packet *receivedPacket = new Packet;
    AuthResult *finalResult = new AuthResult;
    Packet *responsePacket = new Packet;

    int readResult = read(client_socket, receivedPacket, sizeof (Packet));
    if (readResult < 0 || (receivedPacket->type != USERNAME
    && receivedPacket->type != EXIT
    && receivedPacket->type != KEEP_ALIVE
    && receivedPacket->type != HELLO_SERVER)) {
        cout <<"ERRO lendo do socket. Fechando." << endl;
        finalResult->result = ERROR;
        finalResult->sessionAuth = NULL;
        *responsePacket = GlobalManager::commManager.createGenericNackPacket();
    } else if (receivedPacket->type == EXIT) {
        cout <<"Sucesso recebendo EXIT. Fechando." << endl;
        finalResult->result = ERROR;
        finalResult->sessionAuth = NULL;
        *responsePacket = GlobalManager::commManager.createAckPacketForType(EXIT);
    } else if (receivedPacket->type == USERNAME) {
        SessionAuth *sessionAuth = SessionAuth::fromBytes(receivedPacket->_payload);
        finalResult->sessionAuth = new SessionAuth(*sessionAuth);
        cout << "Recebeu dados de autenticação corretamente:" << endl;
        cout << "Tipo " << sessionAuth->getSocketType() << endl;
        cout << "UUID " << sessionAuth->getUuid() << endl;
        cout << "Perfil " << sessionAuth->getProfileId() << endl;

        ErrorCodes sessionCreationResult = GlobalManager::sessionManager.createNewSession(*sessionAuth, client_socket);

        if (sessionCreationResult == ERROR) {
            cout << "ERRO atualizando o usuário. Talvez ele já tenha atingido limite de sessões." << endl;
            finalResult->result = ERROR;
            *responsePacket = GlobalManager::commManager.createGenericNackPacket();

        } else {
            cout << "Usuário atualizado com sucesso" << endl;
            finalResult->result = SUCCESS;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(USERNAME);
        }
    } else if (receivedPacket->type == KEEP_ALIVE) {
        cout << "Keep Alive recebido com sucesso" << endl;
        finalResult->result = SUCCESS;
        finalResult->sessionAuth = NULL;
        *responsePacket = GlobalManager::commManager.createAckPacketForType(KEEP_ALIVE);
    }
    else if (receivedPacket->type == HELLO_SERVER) {
        cout << "HELLO recebido com sucesso" << endl;
        int newProcessID = atoi(receivedPacket->_payload);
        GlobalManager::electionManager.setReceiveSocket(client_socket, newProcessID);
        *responsePacket = GlobalManager::commManager.createAckPacketForType(HELLO_SERVER);
        GlobalManager::commManager.send_packet(client_socket, responsePacket);

        //cria conexão de envio para novo processo se for o caso
        if (!GlobalManager::electionManager.hasValidSendSocketForServer(newProcessID)) {
            cout << "Send Socket invalido. Criando nova conexão" << endl;
            string ip = GlobalManager::electionManager.getIPfromID(newProcessID);
            int sendSocket = connectToServer({.ip= ip, .port=port});
            GlobalManager::electionManager.setSendSocket(sendSocket, newProcessID);

            //manda coordinator se for o caso
            if (GlobalManager::electionManager.isCurrentProcessCoordinator()) {
                GlobalManager::electionManager.sendCoordinatorPacket(sendSocket);
            }
        }

        finalResult->result = SUCCESS;
        finalResult->sessionAuth = NULL;

        *responsePacket = GlobalManager::commManager.createAckPacketForType(HELLO_SERVER);

    }

    cout << "Enviando pacote ACK/NACK recebimento de comando" << endl;
    if (GlobalManager::commManager.send_packet(client_socket, responsePacket) == ERROR) {
        cout << "ERRO enviando ACK/NACK" <<endl;
    } else {
        cout << "ACK/NACK enviado com sucesso" << endl;
    }
    free(receivedPacket);
    free(responsePacket);

    if (finalResult->result == ERROR && finalResult->sessionAuth != NULL) {
        GlobalManager::sessionManager.endSessionWithID(finalResult->sessionAuth->getUuid(),
                                                       finalResult->sessionAuth->getProfileId());
    }

    return (void *) finalResult;

}

void *client_thread_func(void *data) {
    int readResult;
    char buffer[BUFFER_SIZE];
    int _exit = 0;

    SessionAuth *sessionAuthData = (SessionAuth*) data;
    UserInformation userInfo = GlobalManager::sessionManager.getUserByUsername(sessionAuthData->getProfileId());
    Session session = userInfo.getSessionWithID(sessionAuthData->getUuid());

    int commandSocket = session.client_socket;

    // inicia leitura de comandos do cliente
    cout << "Iniciando leitura de comandos do socket " << commandSocket << " do user" << userInfo.username;
    while(!_exit) {
        bzero(buffer, BUFFER_SIZE);

        Packet *receivedPacket = new Packet;
        readResult = read(commandSocket, receivedPacket, sizeof(Packet));

        if (readResult < 0 || !(receivedPacket->type == SEND || receivedPacket->type == FOLLOW || receivedPacket->type == EXIT)) {
            free(receivedPacket);
            printf("ERRO lendo do socket. Desconectando.");
            break;
        }
        cout << "Pacote recebido:" << endl;
        receivedPacket->printItself();
        strcpy(buffer, receivedPacket->_payload);

        Packet *responsePacket = new Packet;

        if(receivedPacket->type == FOLLOW) {
            cout << "Recebeu comando FOLLOW" << endl;
            ErrorCodes followResult = GlobalManager::sessionManager.addNewFollowerToUser(userInfo.username,
                                                                                    receivedPacket->_payload);
            if (followResult == SUCCESS)
                *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
            else
                *responsePacket = GlobalManager::commManager.createGenericNackPacket();

        } else if(receivedPacket->type == SEND) {
            GlobalManager::notifManager.newNotificationSentBy(userInfo.username, receivedPacket->_payload);
            cout << "Recebeu comando SEND" << endl;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
        } else if(receivedPacket->type == EXIT) {
            cout << "Recebeu comando EXIT" << endl;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
            _exit = 1;
        }

        cout << "Enviando pacote ACK/NACK recebimento de comando **" << endl;
        if (GlobalManager::commManager.send_packet(commandSocket, responsePacket) == ERROR) {
            cout << "Não foi possivel enviar ACK/NACK" <<endl;
        } else {
            cout << "ACK/NACK enviado com sucesso" << endl;
        }
        free(receivedPacket);
        free(responsePacket);

    }

    cout << "Termina sessão com usuário" << endl;
    GlobalManager::sessionManager.endSessionWithID(sessionAuthData->getUuid(), sessionAuthData->getProfileId());
    cout << sessionAuthData->getProfileId() << " desconectado" << endl;
    return 0;
}

void closeAppHandler(int n_signal) {
    signal(n_signal, SIG_IGN);
    // salva status dos perfis no arquivo
    cout << "Salvando perfis e notificações...\n";
    fileManager.saveUsersOnFile(GlobalManager::sessionManager.getUsers());
    fileManager.saveNotificationsOnFile(GlobalManager::notifManager.getNotifications());
    cout << "Perfis e notificações salvos!\n";

    Packet exitPacket = GlobalManager::commManager.createExitPacket();
    Packet *packetPointer = (Packet *) malloc(sizeof (Packet));
    *packetPointer = exitPacket;

    list<Session> allSessions = GlobalManager::sessionManager.getAllSessions();
    if (GlobalManager::commManager.sendPacketToSessions(allSessions, packetPointer) == SUCCESS) {
        cout << "Sucesso enviando EXIT para todas as sessões" << endl;
    } else {
        cout << "ERRO enviando EXIT para todas as sessões" << endl;
    }

    GlobalManager::electionManager.sendExitToAllOtherServers();

    GlobalManager::sessionManager.endAllSessions();
    GlobalManager::electionManager.printItself();

    free(packetPointer);

    exit(0);
}
