#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_CLIENTS 4000

// Function to compute factorial
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

	printf("Server Socket is created.\n");

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

	int cnt = 0;
	while (1) {
        socklen_t addr_size = sizeof(cliAddr);
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr,&addr_size);
		// clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr,&addr_size);

		if (clientSocket < 0) {
            perror("Error in accepting");
			exit(1);
		}
		// printf("Connection accepted from %s:%d\n",inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port));
        // printf("Clients connected: %d\n\n",++cnt);
        int pid=fork();
		if (pid == 0) {
            close(sockfd);  
            char buffer[1024];
            while(1)
            {
                int n =recv(clientSocket, &buffer, sizeof(buffer), 0);
                if(n<=0){
                    // printf("Client disconnected\n");
                    close(clientSocket);
                    exit(0);
                }
                int num=atoi(buffer);
                long long ans=fact(num);
                sprintf(buffer, "%llu", ans);    
                int ret=send(clientSocket, &ans, sizeof(ans), 0);
                if(ret>0){

                    // printf("Received: %d and Sent: %s\n", num, buffer);
                }
                else
                    perror("Error in sending");
            }
            close(clientSocket);  
            exit(0); 
        }
        else{
            close(clientSocket);
            if(pid<0)
                perror("fork");
        }
    }

	close(clientSocket);
	return 0;
}
