#include "main.h"

//is list empty
int isEmpty() {
   return headL == NULL;
}

int lengthList() {
   int length = 0;
   struct node *current;
	
   for(current = headL; current != NULL; current = current->next){
      length++;
   }
	
   return length;
}

//insert link at the first location
void insertFirst(int key, pthread_t thread) {

   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
   link->key = key;
   link->thread = thread;
	
   if(isEmpty()) {
      //make it the last link
      last = link;
   } else {
      //update first prev link
      headL->prev = link;
   }

   //point it to old first link
   link->next = headL;
	
   //point first to new first link
   headL = link;
}

//delete a link with given key

struct node* delete(int key) {

   //start from the first link
   struct node* current = headL;
   struct node* previous = NULL;
	
   //if list is empty
   if(headL == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {
      //if it is last node
		
      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;
			
         //move to next link
         current = current->next;             
      }
   }

   //found a match, update the link
   if(current == headL) {
      //change first to point to next link
      headL = headL->next;
   } else {
      //bypass the current link
      current->prev->next = current->next;
   }    

   if(current == last) {
      //change last to point to prev link
      last = current->prev;
   } else {
      current->next->prev = current->prev;
   }
	
   return current;
}

//delete first item
struct node* deleteFirst() {

   //save reference to first link
   struct node *tempLink = headL;
	
   //if only one link
   if(headL->next == NULL){
      last = NULL;
   } else {
      headL->next->prev = NULL;
   }
	
   headL = headL->next;
   //return the deleted link
   return tempLink;
}

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

void FIFO(int socket)
{
    char buffer[30000];

    read(socket, buffer, 30000);

    HTTPRequest request = HTTPRequest_constructor(buffer);
        
        printf("REQUEST\n");
        if (request.Method == GET || request.Method == POST){
            printf("GET OR POST\n");

            if (request.Method == GET){
                char *copyRequestURI = malloc(sizeof(char) * (strlen(request.URI) + 1));
                strcpy(copyRequestURI, request.URI);

                char *parameter = malloc(sizeof(char) * (strlen(request.URI) + 1));
        
                strncpy(parameter, request.URI, 7);

                if(strcmp(parameter, PARAMETERKEY) == 0){
                    char *fileName = malloc(sizeof(char) * (strlen(request.URI) + 1));
                    strncpy(fileName, request.URI+7, strlen(request.URI));

                    responseGet(socket,fileName,FOLDERPATH);   
                    free(fileName);                     
                }
                free(copyRequestURI);
                free(parameter);

            }else if (request.Method == POST){
                responsePost();                 //Codigo para POST
            }
        }
        close(socket);
        strcpy(buffer,"");
}


void forked(int socket,int pid)
{
    printf("Process ID: %d\n",pid);
    FIFO(socket);
    exit(0);
}

void *threaded(void *args)
{
    char buffer[30000];
    int socket = *((int*)args);
    free(args);
    
    read(socket, buffer, 30000);

    HTTPRequest request = HTTPRequest_constructor(buffer);
        
    if (request.Method == GET || request.Method == POST){

        if (request.Method == GET){
            char *copyRequestURI = malloc(sizeof(char) * (strlen(request.URI) + 1));
            strcpy(copyRequestURI, request.URI);

            char *parameter = malloc(sizeof(char) * (strlen(request.URI) + 1));
    
            strncpy(parameter, request.URI, 7);

            if(strcmp(parameter, PARAMETERKEY) == 0){
                char *fileName = malloc(sizeof(char) * (strlen(request.URI) + 1));
                strncpy(fileName, request.URI+7, strlen(request.URI));

                responseGet(socket,fileName,FOLDERPATH);   
                free(fileName);                     
            }
            free(copyRequestURI);
            free(parameter);

        }else if (request.Method == POST){
            responsePost();                 //Codigo para POST
        }
    }
    close(socket);
    free(delete(pthread_self()));
    pthread_exit(0);
}

void *handle_pool(void *args){
    while(kill)
    {
        int *client;
        pthread_mutex_lock(&pool_mutex);
        if((client = dequeue()) == NULL){
            pthread_cond_wait(&pool_cond,&pool_mutex);
            client = dequeue();
        }
        pthread_mutex_unlock(&pool_mutex);
        if(client != NULL){
            int c = *((int*)client);
            FIFO(c);
        }
        free(client);
    }
    pthread_exit(0);
}

void launch(Server *server)
{
    if(serverType.type == 3){
        pool = malloc(serverType.threads*sizeof(pthread_t)); // hay que hacerle free
        for(int i = 0; i < serverType.threads; i++)
            pthread_create(&pool[i],NULL,handle_pool,NULL);
    }
    if(serverType.type == 4){




    }

    else{
        while(kill){
            printf("===== WAITING FOR CONNECTION =====\n");
            int address_length = sizeof(server->address);

            
            int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length);

            if(serverType.type == 1)
                FIFO(new_socket);
            else if(serverType.type == 2)
            {
                pthread_t t;
                int *socket = malloc(sizeof(int));
                *socket = new_socket;
                pthread_create(&t,NULL,threaded,socket);
                insertFirst(t, t);
            }     
            else if(serverType.type == 3)
            {
                int *socket = malloc(sizeof(int));
                *socket = new_socket;
                pthread_mutex_lock(&pool_mutex);
                enqueue(socket);
                pthread_cond_signal(&pool_cond);
                pthread_mutex_unlock(&pool_mutex);
            }
            else if(serverType.type == 4)
            {
                int childpid;
                if((childpid = fork()) == 0){
                    forked(new_socket,getpid());
                }

            }    
        }
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
    }
    return server;
}

void *serverFunc(void *args) {
    IPFinder ipObj = finder_constructor();
    printf("IP: %s\n", ipObj.ip);
    printf("Port: %d\n", PORT);

    Server server = server_constructor(AF_INET,SOCK_STREAM, 0, ipObj.ip, PORT, 10, launch);
    server.launch(&server);
}

void killThreads(){
    if(serverType.type == 3){
        for(int i = 0; i < serverType.threads; i++){
            pthread_cancel(pool[i]);
        }
        free(pool);
    }
    int lengthOfList = lengthList();
    if (lengthOfList > 0)
    {
        for(int i = 0; i < lengthOfList; i++){
            node *threadToDelete = deleteFirst();
            pthread_cancel(threadToDelete->thread);
            free(threadToDelete);
        }
    }
    pthread_cancel(serverThread);//Pthread_cancel
    pthread_exit(0);
}

void killProceces(){
    
}

int main()
{
    kill = 1;
    char consoleInput[30];
    serverType = chooseServer();

    pthread_create(&serverThread,NULL,serverFunc,NULL);

    while(kill){
        scanf("%s", consoleInput);
        if (strcmp(consoleInput, "kill") == 0){
            kill = 0;
        }
        strcpy(consoleInput,"");
    }

    close(server.socket);
    if (serverType.type == 2 || serverType.type == 3){
        killThreads();
    } else if (serverType.type == 4 || serverType.type == 5){
        killProceces();
    }

    return 0;
}