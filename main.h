#ifndef MAIN_h

#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>
#include "HTTPRequest.h"
#include "IPFinder.h"

#define PORT 8000
#define PARAMETERKEY "/?file="
#define FOLDERPATH "./files/"
#define NOTFOUNDPAGEPATH "./files/404.html"

typedef struct OpenedFile
{
    long fsize;
    char * msg;
} OpenedFile;

#endif