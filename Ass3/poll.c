#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 4000

// Function to calculate factorial
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

int main() {
    
	int sockfd, ret,newSocket,client_sockets[MAX_CLIENTS];
	struct sockaddr_in serverAddr;

	int clientSocket;
	struct sockaddr_in cliAddr;
    int addrlen = sizeof(cliAddr);
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
    else{
        perror("Error in listening");
        exit(1);
    }

    // int server_fd, new_socket;
    // struct sockaddr_in address;
    // int addrlen = sizeof(address);

    // if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    //     perror("Socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    // address.sin_port = htons(PORT);

    // if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    //     perror("Bind failed");
    //     exit(EXIT_FAILURE);
    // }

    // if (listen(server_fd, 3) < 0) {
    //     perror("Listen failed");
    //     exit(EXIT_FAILURE);
    // }

    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }

    struct pollfd fds[MAX_CLIENTS + 1];
    memset(fds, 0, sizeof(fds));

    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    while (1) {
        int poll_result = poll(fds, MAX_CLIENTS + 1, -1);

        if (poll_result < 0) {
            perror("Poll error");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            // New connection, accept it
            if ((newSocket = accept(sockfd, (struct sockaddr *)&serverAddr, (socklen_t *)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            for (i = 1; i < MAX_CLIENTS + 1; i++) {
                if (client_sockets[i] == -1) {
                    client_sockets[i] = newSocket;
                    fds[i].fd = newSocket;
                    fds[i].events = POLLIN;
                    break;
                }
            }

            if (i == MAX_CLIENTS + 1) {
                fprintf(stderr, "Too many clients\n");
                close(newSocket);
            }
        }

        for (i = 1; i < MAX_CLIENTS + 1; i++) {
            if (client_sockets[i] != -1 && (fds[i].revents & POLLIN)) {
                char buffer[1024] = {0};
                int ret=read(client_sockets[i], buffer, sizeof(buffer));
                // printf("Read %d bytes\n", ret);
                // perror("Read");
                if(ret<=0){
                    close(client_sockets[i]);
                    // perror("Close");
                    fds[i].fd = -1;
                    client_sockets[i] = -1;
                }
                else{
                    unsigned long long n = atoi(buffer);
                    unsigned long long result = fact(n);
                    char result_str[1024];
                    snprintf(result_str, sizeof(result_str), "%llu", result);
                    // printf("Received: %llu, Sent: %s\n", n, result_str);
                    send(client_sockets[i], result_str, strlen(result_str), 0);
                    // perror("Send");
                }
            }
        }
    }

    return 0;
}
