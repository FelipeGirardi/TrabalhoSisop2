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
#include "../../common/include/Packet.hpp"
#define PORT 4000

using namespace userInformation;
using namespace profileSessionManager;
using namespace communicationManager;

unordered_map<string, UserInformation> users;
unordered_map<string,Notification> notifications;
FileManager fileManager;
ProfileSessionManager sessionManager;
GlobalManager globalManager;
NotificationManager notificationManager;
CommunicationManager comunicationManager;

//TODO: mudar de lugar
typedef struct AuthResult {
    int result; // result < 0 -> Erro. result >= 0 Sucesso!
    string username;
}AuthResult;

// Declaracao funcoes auxiliares
int send_packet(int socket, Packet *package);
void closeAppHandler(int n_signal);
AuthResult authenticate(Session *session);

// Declaracao de funcoes de threads
void *auth_client_func(void *data);
void *client_thread_func(void *data);
void *notification_thread(void *args);



int main(int argc, char* argv[])
{
    cout << "** Primeiro print **\n";

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

    cout << "** Vai criar socket **\n";
    // cria socket TCP verificando erro
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "** Erro abrindo socket **\n";
        return 0;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // seta dados do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(serv_addr.sin_zero), 8);

    cout << "** Vai fazer bind **\n";
    // faz o bind
    if (::bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "** Erro no bind **\n";
        return 0;
    }

    // seta signal de fechar app do servidor
    signal(SIGINT, closeAppHandler);

    cout << "** Vai fazer listen **\n";
    // loop do listen de conexões
    if (listen(sockfd, 5) == 0) {
        int client_sockfd, notif_sockfd; // usar notif_sockfd se precisar
        while(true) {
            // aceita conexão do cliente
            struct sockaddr_in client_address;
            socklen_t client_length = sizeof(struct sockaddr_in);
            cout << "** Vai fazer accept **\n";
            if ((client_sockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_length)) == -1) {
                cout << "** Erro aceitando conexao do cliente **\n";
                continue;
            }

            // accept do canal de notificação vai aqui se precisar
            if ((notif_sockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_length)) == -1) {
                cout << "ERROR on accepting notifications channel\n";
                continue;
            }

            // Creates thread for receiving username

            pthread_t auth_thread;

//            int *result;
            cout << "** Vai criar thread auth **\n";
            Session *newSession = new Session;
            newSession->client_socket = client_sockfd;
            newSession->notif_socket = notif_sockfd;

            cout << "client id new session " << newSession->client_socket << endl;
            cout << "notification id new session " << newSession->notif_socket << endl;
            pthread_create(&auth_thread, NULL, &auth_client_func, (void*)newSession);

            void *resultOfAuthentication;
            pthread_join(auth_thread, &resultOfAuthentication);

            cout << "Result of auth" << ((AuthResult *)resultOfAuthentication)->result << endl;
            AuthResult *myResult = (AuthResult *)resultOfAuthentication;

            if (myResult->result < 0) {
                cout << "Falha na auth" << endl;
                continue;
            } else  {
                cout << "Auth bem sucedida" << endl;
                cout << "new session username = " << newSession->userID << endl;
                // Cria thread para receber comandos do usuario
                pthread_t client_thread;
                cout << "** Vai criar thread cliente **\n";
                pthread_create(&client_thread, NULL, &client_thread_func, (void *) newSession);
            }

            client_sockfd = 0;
            //notifsockfd = 0;
        }
    }

    if (close(sockfd) < 0) {
        cout << "** Erro fechando o socket **" << endl;
    } else {
        cout << "** Sucesso fechando o socket **" << endl;
    }

    return 0;
}

/*
 * Funcao executada por uma thread para pegar o username do usuário
 * Retorno = estrutura AuthResult
 */

void *auth_client_func(void *data) {

    //inicializa estrutura de retorno
    AuthResult *result = new AuthResult;

    //casting de parametros
    Session *session = (Session*) data;
    cout << "client socket auth data" << session->client_socket;

    AuthResult authResult = authenticate(session);
    result->result = authResult.result;
    result->username.assign(authResult.username);

    if (result->result < 0) {
        cout << "\n** Erro na autenticacao **\n";

    } else {
        cout << "Retorno com sucesso da auth" << endl;
        cout << "username = " << result->username << endl;
    }

    cout << "saindo da thread auth" << endl;
    return (void *) result;

}

void *client_thread_func(void *data) {
    int client_socket, bufferInt;  // sem notif_sockfd por enquanto
    char buffer[BUFFER_SIZE];
    int _exit = 0;

    // Extração dos argumentos
    Session *session = (Session*) data;
    client_socket = session->client_socket;

    // inicia leitura de comandos do cliente
    cout << "** Vai iniciar leitura de comandos **\n";
    while(!_exit) {
        bzero(buffer, BUFFER_SIZE);

        Packet *receivedPacket = new Packet;
        bufferInt = read(client_socket, receivedPacket, sizeof(Packet));

        if (bufferInt < 0 || !(receivedPacket->type == SEND || receivedPacket->type == FOLLOW || receivedPacket->type == EXIT)) {
            free(receivedPacket);
            printf("ERROR reading from socket. Disconecting.");
            break;
        }
        cout << "received packet:" << endl;
        receivedPacket->printItself();
        strcpy(buffer, receivedPacket->_payload);

        Packet *responsePacket = new Packet;

        if(receivedPacket->type == FOLLOW) {
            GlobalManager::sessionManager.addNewFollowerToUser(session->userID, receivedPacket->_payload);
            cout << "recebeu follow" << endl;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
        } else if(receivedPacket->type == SEND) {
            GlobalManager::notifManager.newNotificationSentBy(session->userID, receivedPacket->_payload);
            cout << "recebeu send" << endl;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
        } else if(receivedPacket->type == EXIT) {
            cout << "recebeu exit" << endl;
            *responsePacket = GlobalManager::commManager.createAckPacketForType(receivedPacket->type);
            _exit = 1;
        }

        cout << "** Envia pacote ACK/NACK recebimento de comando **" << endl;
        if (GlobalManager::commManager.send_packet(client_socket, responsePacket) < 0) {
            cout << "nao foi possivel enviar" <<endl;
        } else {
            cout << "pacote de response enviado com sucesso" << endl;
        }
        free(receivedPacket);
        free(responsePacket);

    }

    cout << "Termina sessão com usuário" << endl;
    GlobalManager::sessionManager.endSession(*session);
    cout << "** Fecha socket **\n";
    cout << session->userID << " disconnected\n";
    return 0;
}

void closeAppHandler(int n_signal) {
    signal(n_signal, SIG_IGN);
    // salva status dos perfis no arquivo
    cout << "Salvando perfis e notificações...\n";
    fileManager.saveUsersOnFile(GlobalManager::sessionManager.getUsers());
    fileManager.saveNotificationsOnFile(GlobalManager::notifManager.getNotifications());
    cout << "Perfis e notificações salvos!\n";
    exit(0);
}

AuthResult authenticate(Session *session) {

    AuthResult finalResult;
    int readResult;
    char buffer[BUFFER_SIZE];

    // Lendo username
    bzero(buffer, BUFFER_SIZE);

    readResult = read(session->client_socket, buffer, BUFFER_SIZE);
    if (readResult < 0 || buffer[0] == '\0' || buffer[0] == '\n') {
        printf("ERROR reading from socket");
        finalResult.result = -1;
        close(session->client_socket);
        close(session->notif_socket);
        return finalResult;
    }
    cout << "Read username successfully" << endl;
    cout << "buffer = " << buffer << endl;

    // Verificando existência do usuário

    finalResult.username = buffer;
    session->userID.assign(finalResult.username);
    int resultOfSessionCreation = GlobalManager::sessionManager.createNewSession(*session);

    // Criando pacote para enviar
    Packet *responsePacket = new Packet;

    if (resultOfSessionCreation == 1) {
        finalResult.result = 1;
        cout << "success logging user" << endl;
        *responsePacket = GlobalManager::commManager.createAckPacketForType(USERNAME);
    } else {
        *responsePacket = GlobalManager::commManager.createGenericNackPacket();
        finalResult.result = -1;
        cout << " attempted to log but was unsuccessful" << endl;
    }

    // Enviando resposta
    if (GlobalManager::commManager.send_packet(session->client_socket, responsePacket) < 0) {
        finalResult.result = -1;
        //TODO: talvez cortar sessao do usuario
        cout << "nao foi possivel enviar" <<endl;
    } else {
        cout << "enviado" << endl;
    }

    if (finalResult.result == -1) {
        close(session->notif_socket);
        close(session->client_socket);
        cout << "close sockets" << endl;
    }

    free(responsePacket);

    return finalResult;
}

