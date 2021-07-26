/* Code adapted from: https://www.bogotobogo.com/cplusplus/sockets_server_client.php */
/* port number is passed as an argument */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int welcomeSock, connectionSock, portNo;
    socklen_t cliLen;
    char buffer[256];
    struct sockaddr_in servAddr, cliAddr;
    int nBytes;
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /* Create a welcome socket
     * socket(int domain, int type, int protocol)
     */
    welcomeSock = socket(AF_INET, SOCK_STREAM, 0);
    if (welcomeSock < 0)
    {
        error("ERROR opening socket");
    }

    /* Clear addr struct */
    bzero((char *) &servAddr, sizeof(servAddr));

    portNo = atoi(argv[1]);

    /* Setup the host_addr structure for use in the bind call */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;

    /* convert portno to natural byte order */
    servAddr.sin_port = htons(portNo);

    /* bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     * bind() passes file descriptor, the address structure,
     * and the length of the address structure
     * This bind() call will bind the socket to the current IP address on port, portno
     */
    if (bind(welcomeSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        error("ERROR on binding.");
    }

    /* The listen() call tells the socket to listen to the incoming connections.
     * The listen() function places all incoming connection into a backlog queue
     * until accept() call accepts the connection
     * Here, we set the maximum size for the backlog queue to 5.
     */
    listen(welcomeSock, 5);

    /* The accept() call actually accepts an incoming connection */
    cliLen = sizeof(cliAddr);

    /* This accept() function will write the connecting client's address info
     * into the address structure and the size of that structure is clilen.
     * The accept() returns a new socket file descriptor for the accepted connection.
     * So, the original socket file descriptor can continue to be used
     * for accepting new connections while the new socket file descriptor is used for
     * communicating with the connected client.
     */
    connectionSock = accept(welcomeSock, (struct sockaddr *) &cliAddr, &cliLen);

    if (connectionSock < 0) {
        error("ERROR on accept");
    }

    printf("server: got connection from %s at port %d\n",
           inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

    /* This send() function sends the 13 bytes of the string to the new socket. */
    send(connectionSock, "Hello World!\n", 13, 0);

    bzero(buffer, 256);

    nBytes = read(connectionSock, buffer, 255);

    if (nBytes < 0) {
        error("ERROR reading message from socket");
    }

    printf("Here is the message: %s\n", buffer);

    close(connectionSock);
    close(welcomeSock);

    return 0;
}