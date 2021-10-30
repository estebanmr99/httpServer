#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_SIZE 1024

int create_request(char * url, char * port, char * R);
int isValidIP(char * ip);
void * send_request();

char * url, * R, * portNumber;
int T, H;
ssize_t received;
char buffer[BUF_SIZE];

int main(int argc, char**argv) {

    T = atoi(argv[1]);
    H = atoi(argv[2]);
    R = argv[3];
    url = argv[4];
    portNumber = argv[5];

    pthread_t threads[H];

    for (int i = 0; i < H; i++) {
        pthread_create(&threads[i],NULL,send_request,NULL);
    }
    for (int i = 0; i < H; i++) {
        pthread_join(threads[i],NULL);
    }
    return 0;
}

void * send_request(){
    int sockfd;
    long int temp;
    for (int i = 0; i < T; i++) {
        sockfd = create_request(url, portNumber, R);

        memset(&buffer, 0, sizeof(buffer));
        temp = recv(sockfd, buffer, BUF_SIZE, 0);
        while (temp > 0) { //receives the file
            received+=temp;
            temp = recv(sockfd, buffer, BUF_SIZE, 0);
            memset(&buffer, 0, sizeof(buffer));
        }

        printf("\nreceived: %ld\n", received);
        close(sockfd);
    }
}

int create_request(char * url, char * port, char * R) {

    int sockfd;
    char getrequest[2048];
    struct sockaddr_in addr;

    if (isValidIP(url)) { //when an IP address is given
        sprintf(getrequest, "GET /?file=%s HTTP/1.0\nHOST: %s\n\n", R, url);
    }

    // creates a socket to the host
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error creating socket!\n");
        exit(1);
    }
    printf("Socket created...\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(url);
    addr.sin_port = htons(atoi(port));

    //inicia tiempo espera
    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0 ) {
        printf("Connection Error!\n");
        exit(1);
    }
    //termina tiempo espera
    printf("Connection successful...\n\n\n");

    //empieza tiempo atencion
    // writes the HTTP GET Request to the sockfd
    write(sockfd, getrequest, strlen(getrequest));

    return sockfd;
}


int isValidIP(char * ip) {
    struct sockaddr_in addr;
    int valid = inet_pton(AF_INET, ip, &(addr.sin_addr));
    return valid != 0;
}
