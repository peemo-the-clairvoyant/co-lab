#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int cliSock, portNo, nBytes;
    struct sockaddr_in servAddr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
    }

    portNo = atoi(argv[2]);
    cliSock = socket(AF_INET, SOCK_STREAM, 0);
    if (cliSock < 0) {
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR no such host.\n");
        exit(0);
    }

    bzero((char *) &servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,
          (char *) &servAddr.sin_addr.s_addr,
          server->h_length);
    
    servAddr.sin_port = htons(portNo);
    if (connect(cliSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        error("ERROR connecting to the server");
    }

    printf("Please enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    nBytes = write(cliSock, buffer, strlen(buffer));

    if (nBytes < 0) {
        error("ERROR writing to socket.");
    }
    bzero(buffer, 256);
    nBytes = read(cliSock, buffer, 255);

    if (nBytes < 0) {
        error("ERROR reading from socket.");
    }

    printf("%s\n", buffer);
    close(cliSock);

    return 0;
}