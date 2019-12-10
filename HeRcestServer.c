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

int* users_sockets;
char* socket_used;
int nb_of_user;

int getnewsocketpos(){
    int i = 0;
    for(i = 0; i<nb_of_user; i++){
        if(socket_used[i]==0){
            break;
        }
    }/*
    while(i<nb_of_user || socket_used[i]!=0){
        i++;
    }*/
    return i;
}

void sendall(char* msg){
    for(int i = 0; i<nb_of_user; i++){
        //printf("%d\n", users_sockets[i]);
        if(socket_used[i] == 0){
            continue;
        }
        send(users_sockets[i], msg, strlen(msg), 0);
    }
}

void *thread (void* args){
    while(1){
        int* sock = (int*)args;
        char* msg = malloc(sizeof(char)*1024);
        recv(*sock, msg, 1024, 0);
        printf("%s\n", msg);
        //send(*sock, "bonjour", 8, 0);
        sendall(msg);
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]){

    if(argc!=3){
        printf("Usage : %s <port> <number of users>", argv[0]);
        exit(1);
    }

    nb_of_user = atoi(argv[2]);
    users_sockets = malloc(sizeof(int) * nb_of_user);
    memset(users_sockets, 0, sizeof(int) * nb_of_user);
    socket_used = malloc(sizeof(char) * nb_of_user);
    memset(socket_used, 0, sizeof(char) * nb_of_user);

    struct sockaddr_in f;
    int sock = 0, ret_bind = 0;
    memset(&f, 0, sizeof(struct sockaddr_in));

    f.sin_family = AF_INET;
    f.sin_port = htons(atoi(argv[1]));

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Error socket");
        exit(1);
    }
    if((ret_bind = bind(sock, (struct sockaddr*)&f, sizeof(f)))==-1){
        perror("Erreur when binding");
        exit(1);
    }
    listen(sock, 5);
    printf("Listening...\n");
    while(1){
        int clientfd;
        if((clientfd = accept(sock, NULL, NULL)) == -1){
            perror("Error accept");
            exit(1);
        }
        int sockPos = getnewsocketpos();
        //printf("fd : %d, pos : %d\n", clientfd, sockPos);
        users_sockets[sockPos] = clientfd;
        socket_used[sockPos] = 1;
        pthread_t t;
        pthread_create(&t, NULL, thread, &clientfd);
    }
    free(users_sockets);
    close(sock);
}