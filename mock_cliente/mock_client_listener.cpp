#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
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

    //command
    if ((commandsocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
        return 0;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(commandsocket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
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

    while(1) {
        bzero(buffer,256);
        /* read from the socket */
        Packet *pktResponse = new Packet;
        int bufferInt;
        bufferInt = read(notifsocket, pktResponse, sizeof(Packet));
        if (bufferInt < 0) {
            cout << "erro na leitura. Disconnecting" << endl;
            break;
        }
        cout << "leu algo do socket" << endl;

        //n = read(sockfd, buffer, 256);
        cout << "vai printar o pacote"<< endl;
        pktResponse->printItself();
        strcpy(buffer, pktResponse->_payload);
        printf("BUFFER response %s\n",buffer);

        free(pktResponse);
    }

    close(notifsocket);
    close(commandsocket);

    return 0;
}