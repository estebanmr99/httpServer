#include "Server.h"

Server server_constructor(int domain, int service, int protocol,
    u_long interface, int port, int backlog, void (*launch)(Server *server))
{
    Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;

    //The htons function can be used to convert an IP port number in host 
    //byte order to the IP port number in network byte order.
    server.address.sin_port = htons(port);

    //The htonl function can be used to convert an IPv4 address in host 
    //byte order to the IPv4 address in network byte order.
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(domain,service,protocol); //return socket file descriptor
    if(server.socket == 0){
        perror("Failed to connect socket....\n");
        exit(1);
    }

    if((bind(server.socket, (struct sockaddr *)&server.address,sizeof(server.address))) < 0)
    {
        perror("Failed to bind socket....\n");
        exit(1);
    }

    if(listen(server.socket,server.backlog) < 0)
    {
        perror("Failed to start listening....\n");
        exit(1);
    }

    server.launch = launch;

    printf("OK\n");
    return server;
}