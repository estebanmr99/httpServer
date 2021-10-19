#include "main.h"
#include <pthread.h>


typedef struct Params
{
    char* file;
    ServerType type;
    int socket;

}Params;



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

OpenedFile openFile(char *filePath){
    OpenedFile file;
    long fsize;
    FILE *fp = fopen(filePath, "rb");
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

    file.fsize = fsize;
    file.msg = msg;
    return file;
}

char *generateMimeType(const char *extension){
    char *mimeType;

    if(strcmp(extension, ".mp3") == 0){
        mimeType = malloc(sizeof(char) * (strlen("audio/mpeg") + 1));
        strcpy(mimeType, "audio/mpeg");
    } else if(strcmp(extension, ".csv") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/csv") + 1));
        strcpy(mimeType, "text/csv");
    } else if(strcmp(extension, ".mp4") == 0){
        mimeType = malloc(sizeof(char) * (strlen("video/mp4") + 1));
        strcpy(mimeType, "video/mp4");
    } else if(strcmp(extension, ".htm") == 0 || strcmp(extension, ".html") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/html") + 1));
        strcpy(mimeType, "text/html");
    } else if(strcmp(extension, ".jpeg") == 0 || strcmp(extension, ".jpg") == 0){
        mimeType = malloc(sizeof(char) * (strlen("image/jpeg") + 1));
        strcpy(mimeType, "image/jpeg");
    } else if(strcmp(extension, ".png") == 0){
        mimeType = malloc(sizeof(char) * (strlen("image/png") + 1));
        strcpy(mimeType, "image/png");
    } else if(strcmp(extension, ".pdf") == 0){
        mimeType = malloc(sizeof(char) * (strlen("application/pdf") + 1));
        strcpy(mimeType, "application/pdf");
    } else if(strcmp(extension, ".rar") == 0){
        mimeType = malloc(sizeof(char) * (strlen("application/vnd.rar") + 1));
        strcpy(mimeType, "application/vnd.rar");
    } else if(strcmp(extension, ".tar") == 0){
        mimeType = malloc(sizeof(char) * (strlen("application/x-tar") + 1));
        strcpy(mimeType, "application/x-tar");
    } else if(strcmp(extension, ".txt") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/plain") + 1));
        strcpy(mimeType, "text/plain");
    } else if(strcmp(extension, ".css") == 0){
        mimeType = malloc(sizeof(char) * (strlen("text/csv") + 1));
        strcpy(mimeType, "text/css");
    }

    return mimeType;
}

const char *getFilenameExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot;
}

void responseGet(int new_socket, char *fileName, char *path){
    OpenedFile file;
    char clen[40];
    char ctype[40];
    char cdisposition[80];

    printf("Requested file name: %s\n", fileName);
    int newSize = strlen(path)  + strlen(fileName) + 1;     // Determine new size
    char * filePathConcat = (char *)malloc(newSize);    // Allocate new buffer

    // do the copy and concat
    strcpy(filePathConcat, path);
    strcat(filePathConcat, fileName); // or strncat

    if(access(filePathConcat, F_OK ) == 0){
        file = openFile(filePathConcat);

        if (!writeStrToClient(new_socket, "HTTP/1.1 200 OK\r\n")){
            close(new_socket);
        }

        sprintf(clen, "Content-length: %ld\r\n", file.fsize);
        if (!writeStrToClient(new_socket, clen)){
            close(new_socket);
        }

        sprintf(cdisposition, "Content-Disposition: attachment; filename=\"%s\"\r\n", fileName);

        const char *fileExtension = getFilenameExt(fileName);
        char *contentType = generateMimeType(fileExtension);
        sprintf(ctype, "Content-Type: %s\r\n", contentType);

        if (!writeStrToClient(new_socket, cdisposition)){
            close(new_socket);
        }

        if (!writeStrToClient(new_socket, ctype)){
            close(new_socket);
        }

        free(contentType);
    } else {
        file = openFile(NOTFOUNDPAGEPATH);

        if (!writeStrToClient(new_socket, "HTTP/1.1 404 Not Found\r\n")){
            close(new_socket);
        }

        sprintf(clen, "Content-length: %ld\r\n", file.fsize);
        if (!writeStrToClient(new_socket, clen)){
            close(new_socket);
        }

        if (!writeStrToClient(new_socket, "Content-Type: text/html\r\n")){
            close(new_socket);
        }
    }

    if (!writeStrToClient(new_socket, "Connection: close\r\n\r\n") == -1){
        close(new_socket);
    }

    if (!writeDataToClient(new_socket, file.msg, file.fsize)){
        close(new_socket);
    }

    printf("The file was sent successfully\n");
    free(file.msg);
    free(filePathConcat);
}

void responsePost(){

}

void FIFO(int socket, char* file)
{
    responseGet(socket, file, FOLDERPATH);
    free(file);
}

void *threaded(void *params)
{

    Params p = *((Params*)params);

    responseGet(p.socket,p.file,FOLDERPATH);
}

void launch(Server *server, ServerType type)
{
    char buffer[30000];
    while(1){
        printf("===== WAITING FOR CONNECTION =====\n");
        printf("196\n");
        int address_length = sizeof(server->address);
        printf("198\n");
        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);

        ///////---------------------///////

        
        printf("200\n");
        read(new_socket, buffer, 30000);

        HTTPRequest request = HTTPRequest_constructor(buffer);
        
        printf("REQUEST\n");
        if (request.Method == GET || request.Method == POST){
            printf("GET OR POST\n");

            if (request.Method == GET){
                char *copyRequestURI = malloc(sizeof(char) * (strlen(request.URI) + 1));
                strcpy(copyRequestURI, request.URI);
                printf("request %s\n",copyRequestURI);

                char *parameter = malloc(sizeof(char) * (strlen(request.URI) + 1));
        
                strncpy(parameter, request.URI, 7);
                printf("parameter %s\n",parameter);

                if(strcmp(parameter, PARAMETERKEY) == 0){
                    printf("coincide con file=\n");
                    char *fileName = malloc(sizeof(char) * (strlen(request.URI) + 1));
                    strncpy(fileName, request.URI+7, strlen(request.URI));

                    //responseGet(new_socket,fileName,FOLDERPATH);
                    printf("--------------------------------");

                    if(type.type==1)
                        FIFO(new_socket,fileName);

                    else if(type.type==2){
                        pthread_t thread;
                        Params params;
                        params.file = fileName;
                        params.socket = new_socket;
                        params.type = type;
                        pthread_create(&thread,NULL,threaded,&params);
                    }

                    //free(fileName);


                        
                }
                free(copyRequestURI);
                free(parameter);
            }else if (request.Method == POST){
                responsePost();                 //Codigo para POST
            }
        }
        close(new_socket);
        strcpy(buffer,"");
        // free(request.URI);
    }
}



ServerType chooseServer(){

    ServerType server;
    int flag = 1;
    int N,T,P;

    while(flag)
    {
       
        T = 0;
        P = 0;

        scanf("%d",&N);

        if(N>=1 && N<= 5){
            flag=0;
                
            if(N==3)//K hilos
                scanf("%d",&T);

            if(N==5)//K procesos
                scanf("%d",&P);
        server.type = N;
        server.processes = P;
        server.threads = T;   
        }
        else{
            printf("Wrong server option\n");
            sleep(1);
        }

       
        system("clear");
    }



    return server;
}

int main()
{

    ServerType type = chooseServer();
    IPFinder ipObj = finder_constructor();
    printf("IP: %s\n", ipObj.ip);
    printf("Port: %d\n", PORT);

    Server server = server_constructor(AF_INET,SOCK_STREAM, 0, ipObj.ip, PORT, 10, launch);
    server.launch(&server,type);

    close(server.socket);

    return 0;
}