#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>



char* nickname;

void commands(char* cmd){
    if(strcmp(cmd, "exit")){
        exit(EXIT_SUCCESS);
    }
}

void * receiveMessage(void *args){
    while(1){
        int* sock = (int*)args;
        char rec[2048];
        if(recv(*sock, rec, 2048, 0)==-1){
            perror("error receive");
            exit(EXIT_FAILURE);
        }
        printf("\n-----> %s\n", rec);
    }
    pthread_exit(NULL);
}

void *sendMessage(void* args){
    while(1){

    int *sock = (int*)args;
    char *msg = malloc(sizeof(char)*1024);
    printf("%s> ", nickname); 
    scanf("%s", msg);
    if(send(*sock, msg, 1024, 0)==-1){
        perror("send error");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    free(msg);
    }
    
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if(argc!=4){
        printf("Utilisation : connect <host> <port> <nickname>");
        exit(1);
    }
    struct addrinfo hints;
    struct addrinfo* host;
    int sock = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags=AI_CANONNAME;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(argv[1], argv[2], &hints, &host);

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Error socket");
        exit(1);
    }

    nickname = argv[3];

    connect(sock, host->ai_addr, host->ai_addrlen);
    /*send(sock, "oui", 4, 0);
    char *msg = malloc(sizeof(char)*1024);
    printf("send\n");
    recv(sock, msg, 1024, 0);
    printf("%s", msg);*/
    while(1){
        pthread_t threadSend, threadReceive;
        pthread_create(&threadSend, NULL, sendMessage, (void*)&sock);
        pthread_create(&threadReceive, NULL, receiveMessage, (void *)&sock);
        pthread_join(threadSend, NULL);
        //pthread_join(threadReceive, NULL);
    }
    return 0;
}
