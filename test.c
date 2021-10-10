#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>

void launch(Server *server)
{
    char buffer[30000];
    printf("===== WAITING FOR CONNECTION =====");
    int address_length = sizeof(server->address);
    int new_socket = accept(server->socket, 
    (struct sockaddr *)&server->address, (socklen_t *)&address_length);
    read(new_socket, buffer, 3000);
    printf("%s\n",buffer);
    char *hello = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2/14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Length: 88\nContent-Type: text/html\nConnection: Closed\n<html><body><h1><Homo DEus</h1>,</body></html>";
    write(new_socket, hello, strlen(hello));
    close(new_socket);
}
int main()
{
    Server server = server_constructor(AF_INET,SOCK_STREAM, 0,INADDR_ANY, 8080, 10, launch);
    ////
    server.launch(&server);

    return 0;
}