#ifndef Server_h

#define Server_h
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>	//inet_addr
#include <string.h>
#include <sys/types.h>

typedef struct ServerType
{
    int type;
    int threads;
    int processes;
    
}ServerType;

typedef struct Server
{
    int domain;

    int service;

    int protocol;

    char * interface;

    int port;

    int backlog;

    int socket;

    struct sockaddr_in address;    

    void (*launch)(struct Server *server, ServerType type);
}Server;

//declares a pointer to function that takes argument of type void * and returns pointer of type void *
Server server_constructor(int domain, int service, int protocol, char *interface, int port, int backlog, void (*launch)(struct Server *server, ServerType type));

#endif