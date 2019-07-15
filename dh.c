/* Client for 5.1.1-5.1.3 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#define _OPEN_SYS_ITOA_EXT
#define P 97
#define G 15


int modular_pow(int b, int e, int mod){
    int n = b % mod, i;
    for (i = 1; i < e; i++){
        n = (n * b) % mod;
    }
    return n;

}

int main(int argc, char ** argv)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;

    char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc < 4)
    {
        fprintf(stderr, "usage %s selected secret number\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    portno = atoi(argv[2]);

    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system */
    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    /* Building data structures for socket */

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    /* Create TCP socket -- active open
     * Preliminary steps: Setup: creation of active open socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }

    /* Do processing */
    strcpy(buffer,"luomingz\n");
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing B to socket");
        exit(EXIT_FAILURE);
    }
    printf("SENT USERNAME: %s", buffer);

    int b, B, A, s;

    b = atoi(argv[3]);
    printf("b: %d\n",b);

    B = modular_pow(G, b, P);

    sprintf(buffer, "%d\n", B);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing B to socket");
        exit(EXIT_FAILURE);
    }
    printf("SENT B: %s\n", buffer);

    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(EXIT_FAILURE);
    }
    char *ptr;
    A = strtol(buffer, &ptr, 10);
    printf("A: %d\n", A);

    s = modular_pow(A, b, P);

    sprintf(buffer, "%d\n", s);


    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing s to socket");
        exit(EXIT_FAILURE);
    }
    printf("SENT s: %s\n", buffer);

    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(EXIT_FAILURE);
    }
    buffer[n] = 0;
    printf("\n%s\n", buffer);

    close(sockfd);

    return 0;
}



