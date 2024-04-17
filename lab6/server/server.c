#include <stdio.h> 
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
#define MAX_USERS 100
sem_t semathore;
int connections[MAX_USERS];
pthread_t threads[MAX_USERS];
int number_of_users = 0;
int number_of_threads = 0;


void func(int* connfd_p)
{
    int connfd = *connfd_p;
    if (number_of_users == MAX_USERS) {
        return;
    }
    sem_wait(&semathore);
    connections[number_of_users++] = connfd;
    sem_post(&semathore);
    char buff[MAX_BUFF]; 
    bzero(buff, MAX_BUFF); 

    while (read(connfd, buff, sizeof(buff)) != 0) {
        sem_wait(&semathore);
        for (int i = 0; i < number_of_users; i++)
        {
            if (connections[i] != connfd){
                write(connections[i], buff, sizeof(buff));
            }
        }
        sem_post(&semathore);
        bzero(buff, MAX_BUFF); 
    }

    sem_wait(&semathore);
    for (int i = 0; i < number_of_users; i++)
    {
        if (connections[i] == connfd) {
            number_of_users--;
            while (i != number_of_users) {
                connections[i] = connections[i + 1];
                i++;
            }
        }
    }
    sem_post(&semathore);
} 

int main() {
    sem_init(&semathore, 0, 1);

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr)); 
   
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 

    bind(sockfd, &servaddr, sizeof(servaddr));

    while (!listen(sockfd, 5))
    {
        int len = sizeof(cli);
        connfd = accept(sockfd, &cli, &len);
        pthread_create(threads + number_of_threads++, NULL, func, &connfd);
    }

    for (int i = 0; i < number_of_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(sockfd);
    return 0;
}