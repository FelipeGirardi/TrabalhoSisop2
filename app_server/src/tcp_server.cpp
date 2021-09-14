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
#define PORT 4000

using namespace userInformation;
using namespace profileSessionManager;
using namespace communicationManager;

unordered_map<string, UserInformation> users;
FileManager fileManager;
ProfileSessionManager sessionManager;

int send_packet(int socket, Packet *package);
void *auth_client_func(void *data);
void *client_thread_func(void *data);
// void *notification_thread(void *args);
void closeAppHandler(int n_signal);

int main(int argc, char* argv[])
{
    cout << "** Primeiro print **\n";
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
    ClientAuthData client_auth_data;
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

            // Cria thread da autenticação do cliente
//            pthread_t auth_thread;
            client_auth_data.client_socket = client_sockfd;
//            int *result;
//            cout << "** Vai criar thread auth **\n";
            ClientAuthData *clData = (ClientAuthData*) malloc(sizeof (ClientAuthData));
            *clData = client_auth_data;
            cout << "socket id cldata" << clData->client_socket;
            cout << "socket id client_auth data" << client_auth_data.client_socket;
//            pthread_create(&auth_thread, NULL, &auth_client_func, (void*)clData);
//            pthread_join(auth_thread, (void **) &result);
            // TO DO: Checa autenticação
//            if (*result < 0)
//                close(notifsockfd);
//            else {

            // Criando thread para interação com usuário
            pthread_t client_thread;
//            client_auth_data.userID = *result;
            cout << "** Vai criar thread cliente **\n";
            pthread_create(&client_thread, NULL, &client_thread_func, (void *) clData);

//            }

            client_sockfd = 0;
            //notifsockfd = 0;
        }
    }

    if (close(sockfd) < 0) {
        cout << "** Erro fechando o socket **\n";
    }

    return 0;
}

void *auth_client_func(void *data) {
//    int sockfd;
//    int *userID;
//    ClientAuthData *authData = (ClientAuthData*) data;
//
//    sockfd = authData->client_sockfd;
//    users = authData->users;
//
//    // TO DO: autenticação (no CommunicationManager do cliente)
//    //*userID = authenticate(sockfd, users);
//    if (*userID < 0) {
//        cout << "\n** Erro na autenticacao **\n";
//        close(sockfd);
//    }
//
//    pthread_exit(userID);
    return 0;
}

void *client_thread_func(void *data) {
    int client_socket, bufferInt, userID;  // sem notif_sockfd por enquanto
    char buffer[BUFFER_SIZE];
    //UserInformation *currentUser;	    // perfil do usuário da thread
    ClientAuthData *clientData = (ClientAuthData*) data;
    // session_t *currentSession;
    setbuf(stdout, NULL);

    // Extração dos argumentos
    client_socket = clientData->client_socket;
    cout << "socket id" << client_socket <<endl;
    //sockfd_n = clientData->notif_sockfd;
    //users = clientData->users;
    //userID = clientData->userID;

    // TODO: pegar currentUser através do ID (será int ou string?)
    // currentUser = sessionManager.getUserByUsername(username);

    // TODO: adicionar classe session dentro do UserInformation
//    if (cur_user->session_1.isopen == false) {
//        cur_user->session_1.isopen = true;
//        cur_user->session_1.cmdsockfd = sockfd;
//        cur_user->session_1.nsockfd = sockfd_n;
//        cur_session = &(cur_user->session_1);
//    }
//    else if (cur_user->session_2.isopen == false) {
//        cur_user->session_2.isopen = true;
//        cur_user->session_2.cmdsockfd = sockfd;
//        cur_user->session_2.nsockfd = sockfd_n;
//        cur_session = &(cur_user->session_2);;
//    }
//    else {
//        cur_session = NULL;
//    }

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
            cout << "recebeu follow" << endl;
            strcpy(response, "** bombou follow **");
        } else if(strcmp(cmd,"SEND") == 0) {
            //new_notification(profiles, cur_user, data, response);
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
        if (send_packet(client_socket, &package) < 0) {
            break;
        }
        bzero(response, 256);
    }

    //TODO: verificar fechamento da thread
//    cur_user->open_sessions--;
//    cur_session->isopen = false;
//    pthread_cancel(n_thread);
//    close(sockfd_n);
    cout << "** Fecha socket **\n";
    close(client_socket);
//    cout << cur_user->username << " disconnected\n";

    return 0;
}

void closeAppHandler(int n_signal) {
    signal(n_signal, SIG_IGN);
    // salva status dos perfis no arquivo
    cout << "Salvando perfis...\n";
    fileManager.saveUsersOnFile(users);
    //TODO: salvar notificacoes tbm
    cout << "Perfis salvos!\n";
    exit(0);
}

int send_packet(int socket, Packet *package) {

    int n;

    // Enviando metadados
    n = write(socket, package, sizeof(Packet));
    if (n < 0){
        printf("ERROR writing metadata to socket\n");
        n = -1;
    }
    // Enviando payload
    n = write(socket, package->_payload, package->length);
    if (n < 0){
        printf("ERROR writing data to socket\n");
        n = -1;
    }
    return n;

}