#include "HTTPRequest.h"
#include <string.h>
#include <stdlib.h>

int chooseMethod(char *method){

    if (strcmp(method,"GET")==0)
        
        return GET;

    else if(strcmp(method,"POST")==0)
        
        return POST;

    else if(strcmp(method,"PUT")==0)
    
        return PUT;

    else if(strcmp(method,"DELETE")==0)
    
        return DELETE;

    else if(strcmp(method,"PATCH")==0)
        
        return PATCH;

    else if(strcmp(method,"HEAD")==0)
        
        return HEAD;

    else if(strcmp(method,"CONNECT")==0)
        
        return CONNECT;

    else if(strcmp(method,"OPTIONS")==0)
    
        return OPTIONS;

    else if(strcmp(method,"TRACE")==0)
    
        return TRACE;

    else

        return -1;

}

HTTPRequest HTTPRequest_constructor(char *request_str)
{
    
    for(int i = 0; i <= strlen(request_str) - 1; i++)
    {
        if(request_str[i] == '\n' && request_str[i+1] == '\n')
            request_str[i+1] = '|';
    }
    char *request_line = strtok(request_str,"\n");
    char *header_fields = strtok(NULL,"|");
    char *body = strtok(NULL,"|");


    HTTPRequest request;
    char *method = strtok(request_line," ");
    char *URI = strtok(NULL," ");
    char *HTTPVersion = strtok(NULL," ");
    HTTPVersion = strtok(HTTPVersion,"/");
    HTTPVersion = strtok(NULL,"/");
    request.Method = chooseMethod(method);
    request.URI = URI;
    request.HTTPVersion = (float)atof(HTTPVersion);

    return request;

}