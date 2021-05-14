#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#define SIZE_MSG 512

int users_sockfd[100];
pthread_t pthid[100];
int connect_users = 0;

void error(const char *msg) {
	perror(msg);
	exit(1);
}

void delete_userfd(int userfd){
    
    int index = 0;
    for(int i = 0; i <= connect_users; i++){
        if(users_sockfd[i] == userfd){
            index = i;
        }
    }

    for(int i = index;i <= connect_users; i++){
        users_sockfd[i] = users_sockfd[i+1];
        pthid[i] = pthid[i+1];
    }

    pthread_t t;
    pthid[connect_users] = t;
    users_sockfd[connect_users] = 0;
    connect_users--;
}

void* server_handler(void* args){
    int* userfd = (int*)args;
    int test_userfd = *userfd;
    printf("Connect: %d\n", test_userfd);
    char msg[SIZE_MSG];

    while (1){
        if(recv(test_userfd, msg, sizeof(msg), 0) > 0){
            printf("Write user: %d\nMessage: %s", test_userfd, msg);
            for(int i = 0; i < connect_users; i++){
                if(users_sockfd[i] != test_userfd){
                    printf("Send user: %d\n\n", users_sockfd[i], msg);
                    send(users_sockfd[i], msg, sizeof(msg), 0);
                }
            }

            bzero(msg, SIZE_MSG);
        }else{
            printf("Delete: %d\n\n");
            delete_userfd(test_userfd);
            break;
        }
    }
    
}

int main(int argc, char *argv[]) {

    printf("Enter ip: ");
    char ip[17];
    fgets(ip, sizeof(ip), stdin);

    printf("ip: %s", ip);

	int serv_sockfd, clie_sockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;



	serv_addr.sin_addr.s_addr = inet_addr(ip);
	
    serv_addr.sin_port = htons(3425);
	if (bind(serv_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("Error");
	listen(serv_sockfd, 5);
	clilen = sizeof(cli_addr);
    pthread_t tid;

    printf("Server started\n");

    while (1){
        int new_connect = accept(serv_sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        if(new_connect == 0){
            printf("Error accept connect");
            break;
        }else{
            users_sockfd[connect_users] = new_connect;
            connect_users++;
            printf("Client connected!\n");
            printf("UserFD: %d\n", new_connect);
            printf("Users online: %d\n\n", connect_users);
            printf("\n\n\n");
            for(int i = 0; i < connect_users; i++){
                printf("Test: %d\n",users_sockfd[i]);
            }
            pthread_create(&pthid[connect_users], NULL, server_handler, (void *)&new_connect);
            pthread_detach(pthid[connect_users]);
        }
        
    }

	close (serv_sockfd);
	return 0;
}