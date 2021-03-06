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
#include "../app_server/include/utils/StringExtensions.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>

#define PORT 4000

using namespace std;
int socketToSend = -1;
int socketToReceive = -1;
void *sendSomething(void *data);
pthread_t send_thread;
int stop = 0;
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

                cout << "Cancela thread antigo primario" << endl;
                stop = 1;
                sleep(2);
                //pthread_cancel(send_thread);
                cout << "Inicia nova thread" << endl;
                sleep(2);
                stop = 0;
                pthread_t newSendThread;
                send_thread = newSendThread;
                pthread_create(&send_thread, NULL, &sendSomething, NULL);

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



void * sendSomething(void *data) {

    Packet *pkt = new Packet;
    string name;
    string garbage;

    while(!stop) {
        printf("Enter the message: ");
        getline(cin, name);
        StringExtensions stringParser;
        vector <string> splitedString = stringParser.split(name, ' ');
        PacketType command = (PacketType) stoi(splitedString[0]);
        string senderUsername = splitedString[1];
        string commandContent = splitedString[2];

        FrontEndPayload *pktPayload = new FrontEndPayload;
        strncpy(pktPayload->commandContent, commandContent.c_str(), 100);
        strncpy(pktPayload->senderUsername, senderUsername.c_str(), 100);

        cout << "comando = " << stringDescribingType(command) << endl;
        cout << "username = " << pktPayload->senderUsername << endl;
        cout << "command = " << pktPayload->commandContent << endl;

        char *paylooad = pktPayload->toBytes();
        FrontEndPayload *bufferReversed = FrontEndPayload::fromBytes(paylooad);

        cout << "username = " << bufferReversed->senderUsername << endl;
        cout << "command = " << bufferReversed->commandContent << endl;

        bzero(pkt->_payload, 256);
        memcpy(pkt->_payload, paylooad, 256);
        pkt->type = command;
        pkt->length = strlen(paylooad);
        pkt->timestamp = 0;

        FrontEndPayload *newPayload = FrontEndPayload::fromBytes(pkt->_payload);
        cout << "new payload username " << newPayload->senderUsername << endl;
        cout << "new payload content " << newPayload->commandContent << endl;

        /* write in the socket */
        cout << "vai mandar = " << sizeof(*pkt) << endl;
        cout << "SOCKET = " << socketToSend << endl;
        int n = write(socketToSend, pkt, sizeof(Packet));
        if (n < 0)
            printf("ERROR writing to socket\n");
        else
            cout << "deu bom enviando" << endl;
    }
    cout << "retornando da thread" << endl;
    return 0;


    //bzero(buffer,256);
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