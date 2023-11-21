#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8081
#define MAX_CLIENTS 6000

unsigned long long fact(int n) {
    if (n <= 1) return 1;
    if(n>20)
        n=20;

    unsigned long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

void *handler(void *client){
    int clientSocket = (int)client;
    char buffer[1024];
    while(1){
        int n = read(clientSocket, &buffer, 1024);
        if (n <= 0) break;
        buffer[n] = '\0';
        int num = atoi(buffer);
        unsigned long long result = fact(num);
        char res[1024];
        sprintf(res, "%llu", result);
        write(clientSocket, &res, strlen(res));
    }
    close(clientSocket);
    pthread_exit(NULL);
}

int main()
{
	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int clientSocket;
	struct sockaddr_in cliAddr;
	socklen_t addr_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("Error in connection");
		exit(1);
	}

	// printf("Server Socket is created.\n");

	memset(&serverAddr, '\0',
		sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr= INADDR_ANY;

    // int reuse = 1;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    //     perror("setsockopt(SO_REUSEADDR) failed");
    //     exit(1);
    // }

	ret = bind(sockfd,
			(struct sockaddr*)&serverAddr,
			sizeof(serverAddr));
	if (ret < 0) {
		perror("Error in binding");
		exit(1);
	}

	if (listen(sockfd, MAX_CLIENTS) == 0) {
		printf("Listening...\n\n");
	}

    // pthread_t tid[MAX_CLIENTS];
	int cnt = 0;
	while (1) {
        socklen_t addr_size = sizeof(cliAddr);
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr,&addr_size);
		if (clientSocket < 0) {
            perror("Error in accepting");
			exit(1);
		}
        pthread_t tid;
        if (pthread_create(&tid, NULL, handler, clientSocket) != 0) {
            printf("Failed to create thread\n");
        }
        cnt++;
    }
	close(clientSocket);
	return 0;
}
