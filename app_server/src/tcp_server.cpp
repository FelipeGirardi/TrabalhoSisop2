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
#include "../lib/communication.h"
#define PORT 4000

// profile_list *profiles;
// void signalHandler(int);

#define PORT 4000

int main(int argc, char* argv[])
{
	int sockfd, option = 1;
	struct sockaddr_in serv_addr;

    setbuf(stdout, NULL);  // zera buffer do stdout

    // cria socket TCP verificando erro
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("ERROR opening socket\n");
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // seta dados do servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(serv_addr.sin_zero), 8);

    // faz o bind
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding\n");

    // TO DO: carregar estruturas de dados + signal

    // loop do listen de conexões


    // *******************

	listen(sockfd, 5);

	clilen = sizeof(struct sockaddr_in);
	if ((newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen)) == -1)
		printf("ERROR on accept");

	memset(&pkt, 0, sizeof(pkt));

	/* read from the socket */
	n = read(newsockfd, &pkt, sizeof(pkt));
	if (n < 0)
		printf("ERROR reading from socket");
	printf("type: %d\n", pkt.type);
	printf("seqn: %d\n", pkt.seqn);
	printf("length: %d\n", pkt.length);
	printf("timestamp: %d\n", pkt.timestamp);
	printf("payload: %s\n", pkt._payload);

	/* write in the socket */
	n = write(newsockfd, "I got your message", 18);
	if (n < 0)
		printf("ERROR writing to socket");

	close(newsockfd);
	close(sockfd);
	return 0;
}
