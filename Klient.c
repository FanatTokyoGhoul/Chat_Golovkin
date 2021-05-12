#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#define SIZE_MSG 512

char* print_name;

void error(const char *msg) {
	perror(msg);
	exit(1);
}

static void* klient_handler(void* args){
    int* serverfd = (int*)args;
    char msg[SIZE_MSG];
    while (1){

        if(recv(*serverfd, msg, sizeof(msg), 0) > 0){
            printf(msg);
            printf("\n\n");
            printf(print_name);
            bzero(msg, SIZE_MSG);
        }else{
            exit(-1);
        }
    }
    
}

int main(int argc, char *argv[]) {

    printf("Enter name: ");
    char name[100];
    fgets(name, sizeof(name), stdin);


    size_t len = strlen(name);
    if(len > 0 && name[len - 1] == '\n'){
        name[--len] = '\0';
    }

	int clie_sockfd, n;
	char buffer[256];
	//описывает сокет для работы с протоколами IP
	struct sockaddr_in serv_addr;

	struct hostent *server;

	clie_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname("127.0.0.1");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(3425); 
	if(connect(clie_sockfd, (struct sockaddr *) &serv_addr,	sizeof(serv_addr)) < 0)
		error("Error");
	
    print_name = name;
    strncat(print_name, ": ", 3); 

    printf("Fd: %d\n", clie_sockfd);

    pthread_t tid;
    pthread_create(&tid, NULL, klient_handler, (void *)&clie_sockfd);

    char msg[SIZE_MSG];

    send(clie_sockfd, name, sizeof(msg), 0);


    while (1){
        printf(print_name);
        bzero(msg, SIZE_MSG);
        fgets(msg, SIZE_MSG, stdin);
        send(clie_sockfd, msg, sizeof(msg), 0);
    }
    

	close(clie_sockfd);
	return 0;
}