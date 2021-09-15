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
#include "../include/ClientAuthData.hpp"
#include "../include/GlobalManager.hpp"
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

int send_packet(int socket, Packet *package);
void *auth_client_func(void *data);
void *client_thread_func(void *data);
// void *notification_thread(void *args);
void closeAppHandler(int n_signal);
typedef struct AuthResult {
    int result;
    string username;
}AuthResult;
AuthResult authenticate(int clientSocket);



int main(int argc, char* argv[])
{
    cout << "** Primeiro print **\n";

    GlobalManager::sessionManager = sessionManager;
    GlobalManager::notifManager = notificationManager;
    GlobalManager::commManager = comunicationManager;

    users = fileManager.getUsersFromFile();
    notifications = fileManager.getNotificationsFromFile();
    GlobalManager::sessionManager.setUsers(users);
    GlobalManager::notifManager.setNotifications(notifications);

    cout << "users" << endl << endl;

    for (auto user : GlobalManager::sessionManager.getUsers()) {
        UserInformation userInfo = user.second;
        cout << userInfo.toString() << endl;
    }

    cout << "notifications" << endl << endl;

    for (auto notification : GlobalManager::notifManager.getNotifications()) {
        Notification notif = notification.second;
        cout << notif.toString() << endl;
    }

	int sockfd, option = 1;
	struct sockaddr_in serv_addr;

    setbuf(stdout, NULL);  // zera buffer do stdout

    cout << "** Vai criar socket **\n";
    // cria socket TCP verificando erro
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "** Erro abrindo socket **\n";
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // seta dados do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(serv_addr.sin_zero), 8);

    cout << "** Vai fazer bind **\n";
    // faz o bind
    if (::bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        cout << "** Erro no bind **\n";

    // carrega estruturas de dados do arquivo (usuários + notificações se precisar)
    //unordered_map<string,UserInformation> users_retrieval = fileManager.getUsersFromFile();
    //sessionManager.setUsers(users_retrieval);

    //client_auth_data.users = users_retrieval;

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
//            if ((notifsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1) {
//                cout << "ERROR on accepting notifications channel\n";
//                continue;
//            }

            // Creates thread for receiving username
            ClientAuthData client_auth_data;
            pthread_t auth_thread;
            client_auth_data.client_socket = client_sockfd;
//            int *result;
            cout << "** Vai criar thread auth **\n";
            ClientAuthData *clData = new ClientAuthData;
            cout << "retornou do malloc" << endl;
            clData->client_socket = client_auth_data.client_socket;
            cout << "socket id cldata " << clData->client_socket << endl;
            cout << "socket id client_auth data " << client_auth_data.client_socket << endl;
            pthread_create(&auth_thread, NULL, &auth_client_func, (void*)clData);
            void *resultOfAuthentication;
            pthread_join(auth_thread, &resultOfAuthentication);

            cout << "Result of auth" << ((AuthResult *)resultOfAuthentication)->result << endl;
            AuthResult *myResult = (AuthResult *)resultOfAuthentication;

            if (myResult->result < 0) {
                cout << "Na main, falha na auth" << endl;
                break;
            } else  {
                cout << "Na main, auth bem sucedida" << endl;
                cout << "gotten username = " << myResult->username;
                clData->userID.assign(myResult->username);
                cout << "gotten username = " << clData->userID;
                // Creates thread for receiving user's commands
                pthread_t client_thread;
                cout << "** Vai criar thread cliente **\n";
                pthread_create(&client_thread, NULL, &client_thread_func, (void *) clData);
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

void *auth_client_func(void *data) {
    int sockfd;
    AuthResult *result = (AuthResult *) malloc(sizeof (AuthResult));
    cout << "aaaa" << endl;
    ClientAuthData *authData = (ClientAuthData*) data;
    cout << "client socket auth data" << authData->client_socket;
    sockfd = authData->client_socket;
    AuthResult resultado = authenticate(sockfd);
    result->result = resultado.result;
    result->username = resultado.username;

    if (result->result < 0) {
        cout << "\n** Erro na autenticacao **\n";
        close(sockfd);
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
    ClientAuthData *clientData = (ClientAuthData*) data;
    setbuf(stdout, NULL);

    // Extração dos argumentos
    client_socket = clientData->client_socket;
    cout << "socket id" << client_socket <<endl;

    // TO DO: thread das notificações
//    pthread_t n_thread;
//    cout << "Session " << cur_session->id << " opened\n";
//    pthread_create(&n_thread, NULL, &notification_thread, &cur_session);

    // inicia leitura de comandos do cliente
    cout << "** Vai iniciar leitura de comandos **\n";
    while(strcmp(buffer,"exit\n") != 0) {
        bzero(buffer, BUFFER_SIZE);
        //bufferInt = read(client_socket, buffer, BUFFER_SIZE);
        Packet pkt;
        bufferInt = read(client_socket, &pkt, sizeof(Packet));
        if (bufferInt < 0)
            printf("ERROR reading from socket");
        printf("type: %d\n", pkt.type);
        printf("seqn: %d\n", pkt.seqn);
        printf("length: %d\n", pkt.length);
        printf("timestamp: %d\n", pkt.timestamp);
        printf("payload: %s\n", pkt._payload);
        strcpy(buffer, pkt._payload);
        if (bufferInt < 0) {
            cout << "** Erro lendo do socket **\n";
            continue;
        }
        else if (buffer[0] == '\0' || buffer[0] == '\n'){
            cout << "** Nenhuma resposta do cliente **\n";
            cout << buffer << endl;
            cout << "buffer 0 " << buffer[0] << endl;
            break;
        }
//        else
//            cout << cur_user->username << ": " << buffer;

        // Identifica comando digitado pelo cliente
        char *cmd = strtok(buffer," ");
        cout << "COMANDO "<< cmd << endl;
        char *data = strtok(NULL, "\n");
        cout << "data "<< data << endl;
        char response[BUFFER_SIZE] = " ";
        if(strcmp(cmd,"FOLLOW") == 0) {
            //follow(profiles, cur_user, data, response);
            GlobalManager::sessionManager.addNewFollowerToUser(clientData->userID, data);
            cout << "recebeu follow" << endl;
            strcpy(response, "** bombou follow **");
        } else if(strcmp(cmd,"SEND") == 0) {
            //new_notification(profiles, cur_user, data, response);
            GlobalManager::notifManager.newNotificationSentBy(clientData->userID, data);
            cout << "recebeu send" << endl;
            strcpy(response, "** bombou send");
        } else {
            strcpy(response, "** Erro: comando invalido **");
        }

        // Cria pacote da mensagem
        cout << "RESPONSE" << response << endl;
        Packet package;
        package.type = DATA;
        package.seqn = 0;
        package.timestamp = time(NULL);
        //package._payload = response;
        strcpy(package._payload, response);
        package.length = strlen(response);

        cout << "** Envia pacote **\n";
        // Envia pacote da mensagem (TO DO: decidir local da função)
        if (GlobalManager::commManager.send_packet(client_socket, &package) < 0) {
            cout << "nao foi possivel enviar" <<endl;
        }
        cout << "oi1" <<  endl;
        bzero(response, BUFFER_SIZE);
        cout << "oi" <<  endl;

    }

    GlobalManager::sessionManager.endSession(clientData->userID);
//    pthread_cancel(n_thread);
    cout << "** Fecha socket **\n";
    close(client_socket);
    cout << clientData->userID << " disconnected\n";
    return 0;
}

void closeAppHandler(int n_signal) {
    signal(n_signal, SIG_IGN);
    // salva status dos perfis no arquivo
    cout << "Salvando perfis...\n";
    fileManager.saveUsersOnFile(users);
    //fileManager.saveNotificationsOnFile(notifications);
    //TODO: salvar notificacoes tbm
    cout << "Perfis salvos!\n";
    exit(0);
}
AuthResult authenticate(int clientSocket) {

    AuthResult finalResult;
    cout << "authenticate" << endl;
    int n;
    char buffer[BUFFER_SIZE];

    // Lendo username
    bzero(buffer, BUFFER_SIZE);
    cout << "oi client socket = " << clientSocket << endl;
    n = read(clientSocket, buffer, 256);
    if (n < 0) {
        printf("ERROR reading from socket");
        finalResult.result = -1;
        return finalResult;
    }
    cout << "Sucesso na leitura" << endl;
    cout << "buffer = " << buffer << endl;

    // Verificando existência do usuário
    Session newSession;
    newSession.commandSocket = clientSocket;
    n = GlobalManager::sessionManager.createNewSession(buffer, newSession);

    // Criando pacote para enviar
    Packet package;
    package.type = DATA;
    package.seqn = 0;
    package.timestamp = time(NULL);
    bzero(package._payload, BUFFER_SIZE);

    if (n == 1) {
        strcpy(package._payload, "authenticated");
        finalResult.result = 1;
        finalResult.username = buffer;
        cout << "success logging user" << endl;
    } else {
        strcpy(package._payload, "** failed to authenticate **");
        finalResult.result = -1;
        cout << " attempted to log but was unsuccessful" << endl;
    }
    package.length = sizeof (Packet);

    // Enviando resposta
    if (GlobalManager::commManager.send_packet(clientSocket, &package) < 0) {
        cout << "nao foi possivel enviar" <<endl;
    }

    return finalResult;
}

