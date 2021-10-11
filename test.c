#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>
#include "HTTPRequest.h"

void launch(Server *server)
{
    char buffer[30000];
    printf("launcher\n");
    while(1){

        printf("while\n");
        printf("===== WAITING FOR CONNECTION =====\n");
        int address_length = sizeof(server->address);
        int new_socket = accept(server->socket, 
        (struct sockaddr *)&server->address, (socklen_t *)&address_length);
        read(new_socket, buffer, 3000);
        printf("%s\n",buffer);
        char *hello = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2/14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>HELLO WORLD!!!!</h1></body></html>";
        write(new_socket, hello, strlen(hello));
        close(new_socket);
    }
    printf("end while\n");
}
int main()
{
    char input[300];
    strcpy(input,"GET /hello.htm HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\nHost: www.tutorialspoint.com\nAccept-Language: en-us\nAccept-Encoding: gzip, deflate\nConnection: Keep-Alive");
    Server server = server_constructor(AF_INET,SOCK_STREAM, 0,INADDR_ANY, 8000, 10, launch);
    HTTPRequest request = HTTPRequest_constructor(input);

    printf("%f\n",request.HTTPVersion);
    printf("%s\n",request.URI);

    //server.launch(&server);

    return 0;
}