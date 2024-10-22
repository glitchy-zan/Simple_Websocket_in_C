#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVER "127.0.0.1" /* Server IP address */
#define PORT "8080"        /* Server port */

int main()
{
    WSADATA wsaData;
    /*
    struct addrinfo ->
        int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
        int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
        int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
        int              ai_protocol;  // use 0 for "any"
        size_t           ai_addrlen;   // size of ai_addr in bytes
        struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
        char            *ai_canonname; // full canonical hostname
        struct addrinfo *ai_next;      // linked list, next node
    */

    /*
    ---> hints -> tells what we want from getaddrinfo(),
         if field is set to 0 that means we are not filtering based on that field.
         example: hints.ai_socktype = SOCK_STREAM; means we only support TCP and UDP are filtered
    ---> *res  -> is a pointer to results,
    ---> *p    -> is for moving through results
    */
    struct addrinfo hints, *res, *p;
    /* used for socket handle */
    SOCKET socketHandle;
    /* to print the return status of various functions */
    int status;

    /* Initialize Winsock */
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    memset(&hints, 0, sizeof hints); /* initially set all field values inside hints (struct addrinfo) to 0 */
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP */

    /*
    ---> Get address information
    ---> NULL -> creating a server socket and want to listen on all available network interfaces
    ---> PORT -> port number on which you want to listen
    ---> *hints -> specifies the criteria for the socket address information you want
    ---> **res -> pointer to a pointer that will hold the address information returned by getaddrinfo()
         if we just pass res we actually just pass its value that is NULL
         but with passing &res getaddrinfo() sets res to point to first struct in linked list
    */
    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        WSACleanup();
        return 1;
    }

    /* Loop through all the results and bind to the first we can */
    for (p = res; p != NULL; p = p->ai_next)
    {
        if ((socketHandle = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
        {
            perror("socket");
            continue;
        }

        /* Connect to the server */
        if (connect(socketHandle, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR) {
            closesocket(socketHandle);
            perror("connect");
            continue;
        }

        break;
    }

    /* foor loop end and there were no valid sockets */
    if (p == NULL)
    {
        fprintf(stderr, "Failed to bind socket\n");
        WSACleanup();
        return 2;
    }

    printf("Socket successfully connected\n"); /* Successfully connected */
    freeaddrinfo(res);                         /* Free the linked list */

    /* Receive a message from the server */
    char buffer[1024];
    int bytes_received = recv(socketHandle, buffer, sizeof buffer - 1, 0);
    if (bytes_received == SOCKET_ERROR)
    {
        perror("recv");
        WSACleanup();
        return 3;
    }

    buffer[bytes_received] = '\0'; /* Null-terminate the string */
    printf("Received from server: %s", buffer);

    closesocket(socketHandle);
    WSACleanup();
    return 0;
}
