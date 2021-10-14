#ifndef IPFinder_h

#include<stdio.h>	//printf
#include<string.h>	//memset
#include<errno.h>	//errno
#include<sys/socket.h>
#include<netdb.h>
#include<ifaddrs.h>
#include<stdlib.h>
#include<unistd.h>

typedef struct IPFinder
{
    char *ip;
}IPFinder;


//declares a pointer to function that takes argument of type void * and returns pointer of type void *
IPFinder finder_constructor();

#endif