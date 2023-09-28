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

#define PORT 8080
#define MAXCONNECTIONS 10

fd_set socks;
int sock, highSockFdVal;
int connectlist[MAXCONNECTIONS];

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
    int listnum; // Current item in connectlist for for loops
    // Zero out the fd_set - make sure it's pristine
    FD_ZERO(&socks);
    // Set the fd_set to include listener
    FD_SET(listener, &socks);
    // Now check each slot to see if it's occupied
    for (listnum = 0; listnum < MAXCONNECTIONS; listnum++) {
        if (connectlist[listnum] != 0) {
            // If we set the fd_set, we have to check it when we get sockets
            FD_SET(connectlist[listnum], &socks);
            // This will be the largest file descriptor
            // Or is it?  Keep track of it and only use when select() tells us
            if (connectlist[listnum] > highsock) {
                highsock = connectlist[listnum];
            }
        }
    }
}   

int main(void) {
    
    return (0);
}