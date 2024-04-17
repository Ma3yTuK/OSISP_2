#include <stdio.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h> 
#include <unistd.h>


const int PORT = 8080;
#define MAX_BUFF 1024
char name[MAX_BUFF];


void send_func(int* sock) {
    int sockfd = *sock;
    char buff[MAX_BUFF];
    char tmp_name[MAX_BUFF];
    bzero(tmp_name, MAX_BUFF);
    bzero(buff, MAX_BUFF);
    scanf("%[^\n]s", buff);
    getchar();
    strcpy(tmp_name, name);
    strcat(tmp_name, ": ");
    strcat(tmp_name, buff);

    while (write(sockfd, tmp_name, sizeof(tmp_name)) != 0) {     
        bzero(tmp_name, MAX_BUFF);
        bzero(buff, MAX_BUFF);
        scanf("%[^\n]s", buff);
        getchar();
        strcpy(tmp_name, name);
        strcat(tmp_name, ": ");
        strcat(tmp_name, buff);
    }
}


void recv_func(int* sock) {
    int sockfd = *sock;
    char buff[MAX_BUFF];
    bzero(buff, MAX_BUFF); 

    while (read(sockfd, buff, sizeof(buff)) != 0)
    {
        printf("%s\n", buff);
        bzero(buff, MAX_BUFF); 
    }
}


int main() {
    printf("Enter name: ");
    scanf("%[^\n]s", name);
    getchar();
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, &servaddr, sizeof(servaddr)))
    {
        printf("Failed connection\n");
        return 0;
    }

    pthread_t w, r;
    pthread_create(&w, NULL, send_func, &sockfd);
    pthread_create(&r, NULL, recv_func, &sockfd);

    pthread_join(w, NULL);
    pthread_join(r, NULL);
    
    close(sockfd);
}