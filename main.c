#include <stdio.h>
#include "Server.h"
#include <string.h>
#include <unistd.h>
#include "HTTPRequest.h"
#include "IPFinder.h"

int writeDataToClient(int sckt, const void *data, int datalen)
{
    const char *pdata = (const char*) data;

    while (datalen > 0){
        int numSent = send(sckt, pdata, datalen, 0);
        if (numSent <= 0){
            if (numSent == 0){
                printf("The client was not written to: disconnected\n");
            } else {
                perror("The client was not written to");
            }
            return 0;
        }
        pdata += numSent;
        datalen -= numSent;
    }

    return 1;
}

int writeStrToClient(int sckt, const char *str)
{
    return writeDataToClient(sckt, str, strlen(str));
}

void sendFile(int new_socket, char *fileName, char *path){
    printf("Requested file name: %s\n", fileName);
    
    // Determine new size
    int newSize = strlen(path)  + strlen(fileName) + 1; 

    // Allocate new buffer
    char * filePathConcat = (char *)malloc(newSize);

    // do the copy and concat
    strcpy(filePathConcat, path);
    strcat(filePathConcat, fileName); // or strncat

    if(access(filePathConcat, F_OK ) == 0){
        long fsize;
        FILE *fp = fopen(filePathConcat, "rb");
        if (!fp){
            perror("The file was not opened");    
            exit(1);    
        }

        if (fseek(fp, 0, SEEK_END) == -1){
            perror("The file was not seeked");
            exit(1);
        }

        fsize = ftell(fp);
        if (fsize == -1) {
            perror("The file size was not retrieved");
            exit(1);
        }
        rewind(fp);

        char *msg = (char*) malloc(fsize);
        if (!msg){
            perror("The file buffer was not allocated\n");
            exit(1);
        }

        if (fread(msg, fsize, 1, fp) != 1){
            perror("The file was not read\n");
            exit(1);
        }
        fclose(fp);

        printf("The file size is %ld\n", fsize);

        if (!writeStrToClient(new_socket, "HTTP/1.1 200 OK\r\n")){
            close(new_socket);
        }

        char clen[40];
        sprintf(clen, "Content-length: %ld\r\n", fsize);
        if (!writeStrToClient(new_socket, clen)){
            close(new_socket);
        }

        if (!writeStrToClient(new_socket, "Content-Type: text/html\r\n")){
            close(new_socket);
        }

        if (!writeStrToClient(new_socket, "Connection: close\r\n\r\n") == -1){
            close(new_socket);
        }

        if (!writeDataToClient(new_socket, msg, fsize)){
            close(new_socket);
        }

        printf("The file was sent successfully\n");
    }

}

void launch(Server *server)
{
    char buffer[30000];
    while(1){
        printf("===== WAITING FOR CONNECTION =====\n");
        int address_length = sizeof(server->address);
        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);
        read(new_socket, buffer, 30000);

        HTTPRequest request = HTTPRequest_constructor(buffer);
        
        if (request.Method == GET || request.Method == POST){
            char *copyRequestURI = malloc(sizeof(char) * (strlen(request.URI) + 1));
            strcpy(copyRequestURI, request.URI);

            char *parameter = malloc(sizeof(char) * (strlen(request.URI) + 1));
            strncpy(parameter, request.URI, 7);

            if(strcmp(parameter, "/?file=") == 0){
                char *fileName = malloc(sizeof(char) * (strlen(request.URI) + 1));
                strncpy(fileName, request.URI+7, strlen(request.URI));
                
                sendFile(new_socket, fileName, "./files/");
            }
        }
        close(new_socket);
        strcpy(buffer,"");
    }
}

int main()
{
    IPFinder test = finder_constructor();
    int port = 8000;
    printf("IP: %s\n", test.ip);
    printf("Port: %d\n", port);

    Server server = server_constructor(AF_INET,SOCK_STREAM, 0, test.ip, port, 10, launch);

    server.launch(&server);

    return 0;
}