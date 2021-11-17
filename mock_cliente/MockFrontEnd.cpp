#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../common/include/Packet.hpp"
#include "../common/include/PacketType.hpp"
#include "../common/include/FrontEndPayload.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 4000

using namespace std;
int socketToSend = -1;
int socketToReceive = -1;
void sendSomething();
void *auth_client_func(void *data);

int main(int argc, char *argv[])
{
    string serverIP = "127.0.0.10";
    int sockfd, option = 1;
    struct sockaddr_in serv_addr;

    setbuf(stdout, NULL);  // zera buffer do stdout

    cout << "Criando socket front end\n";
    // cria socket TCP verificando erro
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "ERRO abrindo socket **\n";
        return 0;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // seta dados do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    bzero(&(serv_addr.sin_zero), 8);

    // faz o bind
    cout << "Fazendo bind IP e porta\n";
    if (::bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ERRO no bind **\n";
        return 0;
    }

    // ignora o signal do write em socket fechado
    signal (SIGPIPE, SIG_IGN);

    // loop do listen de conexões
    if (listen(sockfd, 5) == 0) {
        int client_sockfd;

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

            if (socketToSend != -1) {
                sendSomething();
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

void sendSomething() {

    char buffer[256];
    Packet *pkt = new Packet;

    FrontEndPayload *pktPayload = new FrontEndPayload;
    strncpy(pktPayload->commandContent, "@marioooooo", 100);
    strncpy(pktPayload->senderUsername, "@mariaaaaaa", 100);

    cout << "username = " << pktPayload->senderUsername << endl;
    cout << "command = " << pktPayload->commandContent << endl;

    char * paylooad = pktPayload->toBytes();
    FrontEndPayload *bufferReversed = FrontEndPayload::fromBytes(paylooad);

    cout << "username = " << bufferReversed->senderUsername << endl;
    cout << "command = " << bufferReversed->commandContent << endl;

    bzero(pkt->_payload, 256);
    memcpy(pkt->_payload, paylooad, 256);
    pkt->type = FOLLOW;
    pkt->length = strlen(paylooad);
    pkt->timestamp = 0;

    FrontEndPayload *newPayload = (FrontEndPayload *) pkt->_payload;
    cout << "new payload username " << newPayload->senderUsername << endl;
    cout << "new payload content " << newPayload->commandContent << endl;

    /* write in the socket */
    cout << "vai mandar\n" << endl;
    int n = write(socketToSend, &pkt, sizeof(pkt));
    if (n < 0)
        printf("ERROR writing to socket\n");
    else
        cout << "deu bom enviando";

    bzero(buffer,256);
}

Packet createAckPacketForType(PacketType type) {

    cout << "Criando pacote ACK" << endl;
    string responseString = "Uhu! pacote recebido com sucesso! :)";

    Packet package;
    package.type = SERVER_ACK;
    package.timestamp = time(NULL);
    bzero(package._payload, BUFFER_SIZE);
    strncpy(package._payload, responseString.c_str(), BUFFER_SIZE);
    package.length = strlen(package._payload);
    return package;
}

int send_packet(int socket, Packet* package) {
    cout << "Enviando o pacote:" << endl;
    int aa = write(socket, package, sizeof(Packet));
    cout << "valor de retorno do write = " << aa << endl;

    if (aa < 0) {
        cout << "ERRO escrevendo no socket" << endl;
        return -1;
    }
    return 0;
}

/*
 * Funcao executada por uma thread para pegar o username do usuário
 * Retorno = estrutura AuthResult
 */

void *auth_client_func(void *data) {

    int *receivedSocket = (int*) data;
    Packet *responsePacket = new Packet;
    int client_socket = *receivedSocket;
    int shouldSendCoordinator = false;
    cout << "Iniciando authenticação do socket = " << client_socket << endl;
    Packet *receivedPacket = new Packet;

    int readResult = read(client_socket, receivedPacket, sizeof (Packet));
    if (readResult < 0 || (receivedPacket->type != HELLO_SEND
                           && receivedPacket->type != HELLO_RECEIVE)) {
        cout <<"ERRO lendo do socket. Fechando." << endl;
    }  if (receivedPacket->type == HELLO_SEND) {
        cout << "HELLO SEND recebido com sucesso" << endl;
        socketToReceive = client_socket;
        *responsePacket = createAckPacketForType(HELLO_SEND);

    }
    else if (receivedPacket->type == HELLO_RECEIVE) {
        cout << "HELLO RECEIVE recebido com sucesso" << endl;
        socketToSend = client_socket;
        *responsePacket = createAckPacketForType(HELLO_RECEIVE);
    }

    cout << "Enviando pacote ACK/NACK para primario" << endl;
    if (send_packet(*receivedSocket, responsePacket) == -1) {
        cout << "ERRO enviando ACK/NACK" << endl;
    } else {
        cout << "ACK/NACK enviado com sucesso" << endl;
    }

    return 0;

}