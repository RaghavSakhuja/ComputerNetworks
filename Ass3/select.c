#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_CLIENTS 1024
#define PORT 8080

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
    int server_socket, new_socket, ret, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr;
    int opt = 1;

    int max_socket, activity, i, valread;
    int addrlen = sizeof(server_addr);

    char buffer[1025];

    fd_set read_fds;

    for (i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    ret = bind(server_socket,
			(struct sockaddr*)&server_addr,
			sizeof(server_addr));
	if (ret < 0) {
		perror("Error in binding");
		exit(1);
	}
    printf("Server socket created\n");

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");


    while (1) {
    
        max_socket = server_socket; 
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);

        for (i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
            }

            if (client_sockets[i] > max_socket) {
                max_socket = client_sockets[i];
            }
        }

        activity = select(max_socket + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select error");
        }

        if (FD_ISSET(server_socket, &read_fds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr*)&server_addr, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            int client_socket = client_sockets[i];
            if (FD_ISSET(client_socket, &read_fds)) {
                if(valread=read(client_socket, buffer, 1024)) {
                    buffer[valread] = '\0';
                    int num = atoi(buffer);
                    unsigned long long result = fact(num);
                    char res[1024];
                    sprintf(res, "%llu", result);
                    send(client_socket, &res, strlen(res), 0);
                }
                else {
                    getpeername(client_socket, (struct sockaddr*)&server_addr, (socklen_t*)&addrlen);
                    // printf("Host disconnected, ip %s, port %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
                    close(client_socket);
                    client_sockets[i] = 0;
                }
            }
        }
    }


    return 0;
}
