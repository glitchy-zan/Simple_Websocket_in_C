#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT "8080"

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

        // Bind the socket
        if (bind(socketHandle, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR)
        {
            closesocket(socketHandle);
            perror("bind");
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

    printf("Socket successfully bound\n"); /* Successfully bound */
    freeaddrinfo(res);                     /* Free the linked list */

    /* Listen for incoming connections. Socket is listening until stopped or error or progrem termination. */
    if (listen(socketHandle, SOMAXCONN) == SOCKET_ERROR)
    {
        perror("listen");
        WSACleanup();
        return 3;
    }

    printf("Server listening on port %s\n", PORT);

    struct sockaddr_storage client_addr;      /* stores addres of a client */
    socklen_t addr_size = sizeof client_addr; /* size of that addres */
    /*
        accepting client request
        (struct sockaddr *)&client_addr -> accept function expects 'sockaddr' pointer so we need
        to cast our 'sockaddr_storage'. We want to use 'sockaddr_storage' because it is more
        flexible and can store various addreses
    */
    SOCKET new_sock = accept(socketHandle, (struct sockaddr *)&client_addr, &addr_size);

    if (new_sock == INVALID_SOCKET)
    {
        perror("accept");
        WSACleanup();
        return 4;
    }

    /* Send a welcome message to the client */
    char *welcome_msg = "Hello, client!\n";
    send(new_sock, welcome_msg, strlen(welcome_msg), 0);

    closesocket(new_sock);
    closesocket(socketHandle);
    WSACleanup();
    return 0;
}
