#ifndef MAIN_h

#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>
#include "HTTPRequest.h"
#include "IPFinder.h"
#include <pthread.h>
#include "Queue.h"

#define PORT 8080
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

typedef struct node {
   pthread_t thread;
   pid_t pid;
   int key;
	
   struct node *next;
   struct node *prev;
}node;


pthread_t *pool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;
pthread_t serverThread;
ServerType serverType;
Server server;
int killFlag;

//this link always point to first Link
struct node *headL = NULL;
//this link always point to last Link 
struct node *last = NULL;
struct node *current = NULL;


#endif