#ifndef MAIN_h

#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>
#include "HTTPRequest.h"
#include "IPFinder.h"
#include <pthread.h>
#include "Queue.h"

#define PORT 8000
#define PARAMETERKEY "/?file="
#define FOLDERPATH "./files/"
#define NOTFOUNDPAGEPATH "./files/404.html"

typedef struct OpenedFile
{
    long fsize;
    char * msg;
} OpenedFile;

typedef struct Args
{
    ServerType type;
    int socket;

}Args;


pthread_t *pool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;

#endif