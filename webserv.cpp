#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/time.h>

// #define PORT 80802// #define 10 10

fd_set socks;
int listener , highSockFdVal;
int socketList[10];

void setNonBlockingSocket(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("F_GETFL");
        exit(1);
    }
    int res = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (res == -1) {
        perror("F_SETFL");
        exit(1);
    }
    return;
}

void build_select_list() {
    printf("Building select list\n");
    FD_ZERO(&socks);
    FD_SET(listener, &socks); // add the server's listening socket
    for (int i = 0; i < 10; i++) {
        if (socketList[i] != 0) {
            FD_SET(socketList[i], &socks);
            if (socketList[i] > highSockFdVal) {
                highSockFdVal = socketList[i];
            }
        }
    }
}   

void acceptNewConnection() {
    printf("New connection received\n");
    int newConnection = -1;
    newConnection = accept(listener, NULL, NULL);
    if (newConnection < 0) {
        perror("Connection");
        exit(EXIT_FAILURE);
    }
    setNonBlockingSocket(newConnection);
    int index = 0;
    for (; index < 10; index++)
    {
        if(socketList[index] == 0) {
            socketList[index] = newConnection;
            printf("Adding client on fd %d\n", newConnection);
            break;
        }
    }
    if(index == 10) {
        printf("Too many clients try in sometime..\n");
        close(newConnection);
        exit(EXIT_FAILURE);
    }
}   

void handleClientReq() {
    char buffer[1024];
    int index = 0;
    for(; index < 10; index++) {
        if(FD_ISSET(socketList[index], &socks)) {
            if(recv(socketList[index], buffer, sizeof(buffer), 0) == 0) {
                close(socketList[index]);
                socketList[index] = 0;
                printf("Client on fd %d disconnected\n", socketList[index]);
            } else {
                printf("Client on fd %d says %s\n", socketList[index], buffer);
                const char *response = "HTTP/1.1 200 OK\r\nDate: Sat, 24 Sep 2023 12:00:00 GMT\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Sample Page</title>\r\n<style>body {background-color: #f0f0f0;margin: 0;padding: 0;}h1 {color: blue;}p {color: red;}</style>\r\n</head>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n<p>This is a sample page.</p>\r\n</body>\r\n</html>\r\n";
                send(socketList[index], response, strlen(response), 0);
                close(socketList[index]);
                socketList[index] = 0;
            }
        }        
    }
}

void runServer() {
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int addrLen = sizeof(struct sockaddr_in);
    int newConnection = -1;
    int index = 0;
    for(; index < 10; index++) {
        socketList[index] = 0;
    }
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    setNonBlockingSocket(listener);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    if(listen(listener, 10) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d\n", 8080);
    while(1) {
        build_select_list();
        int n = select(1024 + 1, &socks, (fd_set *)0, (fd_set *)0, NULL);
        printf("Heree");
        if(n < 0) {
            perror("Select");
            exit(EXIT_FAILURE);
        }
        if(FD_ISSET(listener, &socks)) {
            acceptNewConnection();
        } else {
            handleClientReq();
        }
    }
}

int main() {
    runServer();
    return 0;
}
