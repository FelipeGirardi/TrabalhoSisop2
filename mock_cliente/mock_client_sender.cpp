#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../common/include/Packet.hpp"
#include "../common/include/PacketType.hpp"

#define PORT 4000

using namespace std;

int main(int argc, char *argv[])
{
    int commandsocket, notifsocket, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    Packet pkt;

    char buffer[256];
    if (argc < 2) {
        fprintf(stderr,"usage %s hostname\n", argv[0]);
        exit(0);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    //comandos
    if ((commandsocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening command socket\n");
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(commandsocket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting to command socket\n");
        return 0;
    }

    //notificacoes
    if ((notifsocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening notif socket\n");
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(notifsocket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting to notif socket\n");
        return 0;
    }

    //nome
    bzero(buffer,256);
    printf("Enter the name: ");
    fgets(buffer, 256, stdin);
    n = write(commandsocket, buffer, 256);
    if (n < 0) {
        printf("ERROR writing name to socket\n");
        return 0;
    }

    bzero(buffer,256);

    /* read from the socket */
    Packet *usernameResponsePKT = new Packet;
    int usernameResponse = read(commandsocket, usernameResponsePKT, sizeof(Packet));
    if (usernameResponse < 0) {
        printf("ERROR reading username response from socket\n");
        close(commandsocket);
        close(notifsocket);
        return 0;
    } else {
        usernameResponsePKT->printItself();
    }

    while(1) {
        printf("Enter the message: ");
        bzero(pkt._payload, 256);
        fgets(pkt._payload, sizeof(pkt._payload), stdin);
        pkt._payload[strcspn(pkt._payload, "\n")] = 0;
        cout << "packet after strcspn" << pkt._payload << endl;
        pkt.type = SEND;
        pkt.length = strlen(pkt._payload);
        pkt.timestamp = 0;
        printf("payload %s\n", pkt._payload);

        /* write in the socket */
        cout << "vai mandar" << endl;
        n = write(commandsocket, &pkt, sizeof(pkt));
        if (n < 0)
            printf("ERROR writing to socket\n");

        bzero(buffer,256);

        /* read from the socket */
        Packet *pktResponse = new Packet;
        cout << sizeof(Packet);
        int bufferInt;
        bufferInt = read(commandsocket, pktResponse, sizeof(Packet));
        if (bufferInt < 0) {
            printf("ERROR reading from socket\n");
            return 0;
        }
        //n = read(sockfd, buffer, 256);
        cout << "conseguiu ler resposne" << endl;
        cout << "vai printar response" << endl;
        pktResponse->printItself();
        strcpy(buffer, pktResponse->_payload);
        printf("BUFFER %s\n",buffer);
    }

    close(commandsocket);
    close(notifsocket);
    return 0;
}