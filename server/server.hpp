#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h> 

#define TRUE            1
#define FALSE           0
#define MAX_CONNECTIONS 42
#define PORT            8080

int client_sockets_FD[MAX_CONNECTIONS];
int master_socket;
int opt = 1;
int highest_fd_val;
socklen_t addrlen;

struct sockaddr_in address;  
fd_set read_fds;

#endif