#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 4000

typedef struct __packet{
    uint16_t type; //Tipo do pacote (p.ex. DATA | CMD)
    uint16_t seqn; //Número de sequência
    uint16_t length; //Comprimento do payload
    uint16_t timestamp; // Timestamp do dado
    char _payload [256]; //Dados da mensagem
} packet;

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    packet pkt;

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

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        printf("ERROR opening socket\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);


    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        return 0;
    }

    bzero(buffer,256);
    printf("Enter the name: ");
    fgets(buffer, 256, stdin);

    n = write(sockfd, buffer, 256);
    if (n < 0) {
        printf("ERROR writing name to socket\n");
        return 0;
    }

    while(1) {
        bzero(buffer,256);
        /* read from the socket */
        packet pktResponse;
        int bufferInt;
        bufferInt = read(sockfd, &pktResponse, sizeof(packet));
        //n = read(sockfd, buffer, 256);
        strcpy(buffer, pktResponse._payload);
        if (bufferInt < 0)
            printf("ERROR reading from socket\n");
        else
            printf("BUFFER response %s\n",buffer);
    }

    close(sockfd);
    return 0;
}