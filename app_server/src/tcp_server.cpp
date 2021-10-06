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
#include <string>
#include <unordered_map>
#include "../include/profile_session_manager.hpp"
#include "../include/user_information.hpp"
#include "../include/FileManager.hpp"
#include "../include/CommunicationManager.hpp"
#include "../include/Session.hpp"
#include "../include/GlobalManager.hpp"
#include "../include/utils/ErrorCodes.hpp"
#include "../../common/include/Packet.hpp"
#include "../../common/include/SessionAuth.hpp"
#define PORT 4000

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
bool shouldEnd = false;

//TODO: mudar de lugar
typedef struct AuthResult {
    ErrorCodes result;
    SessionAuth *sessionAuth;
}AuthResult;

// Declaracao funcoes auxiliares
void closeAppHandler(int n_signal);

// Declaracao de funcoes de threads
void *auth_client_func(void *data);
void *client_thread_func(void *data);

int main(int argc, char* argv[])
{

    //instanciação dos managers
    GlobalManager::sessionManager = sessionManager;
    GlobalManager::notifManager = notificationManager;
    GlobalManager::commManager = comunicationManager;

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
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(serv_addr.sin_zero), 8);

    // faz o bind
    cout << "Fazendo bind IP e porta\n";
    if (::bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ERRO no bind **\n";
        return 0;
    }

    // seta signal de fechar app do servidor
    signal(SIGINT, closeAppHandler);

    // loop do listen de conexões
    if (listen(sockfd, 5) == 0) {
        int client_sockfd, notif_sockfd; // usar notif_sockfd se precisar

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

            string username = myResult->sessionAuth->getProfileId();

            if (myResult->result == SUCCESS) {
                if (myResult->sessionAuth->getSocketType() == COMMAND_SOCKET) {
                    pthread_t client_thread;
                    cout << "Criando thread de leitura de comandos" << endl;
                    SessionAuth *pointerToSessionAuth = myResult->sessionAuth;
                    pthread_create(&client_thread, NULL, &client_thread_func, (void *) pointerToSessionAuth);
                } else {

                    GlobalManager::sessionManager.users[username]
                    .startListeningForNotifications();

                }
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
    if (readResult < 0 || (receivedPacket->type != USERNAME  && receivedPacket->type != EXIT)) {
        cout <<"ERRO lendo do socket. Fechando." << endl;
        finalResult->result = ERROR;
        finalResult->sessionAuth = NULL;
        *responsePacket = GlobalManager::commManager.createGenericNackPacket();
    } else if (receivedPacket->type == EXIT) {
        cout <<"Sucesso recebendo EXIT. Fechando." << endl;
        finalResult->result = ERROR;
        finalResult->sessionAuth = NULL;
        *responsePacket = GlobalManager::commManager.createAckPacketForType(EXIT);
    } else {
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

    GlobalManager::sessionManager.endAllSessions();
    free(packetPointer);

    exit(0);
}
