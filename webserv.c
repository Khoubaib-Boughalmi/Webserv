#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h> 

#define PORT            8080
#define TRUE            1
#define FALSE           0
#define MAX_CONNECTIONS 42

int client_sockets_FD[MAX_CONNECTIONS];
int master_socket, opt = 1;
int highest_fd_val;

struct sockaddr_in address;  
fd_set read_fds;

void set_non_blocking_socket(int fd) {
    int flags = fcntl(fd, F_GETFD, 0) ;
    if (flags <= 0) {
        perror("F_GETFD:");
        exit(EXIT_FAILURE);
    }
    int res = fcntl(fd, F_SETFD, flags | O_NONBLOCK) ;
    if(res <= 0) {
        perror("F_SETFD:");
        exit(EXIT_FAILURE);
    }
}

void initialization_and_socket_creation (void) {
    //initialize client_sockets_FD
    for ( int i = 0; i < MAX_CONNECTIONS; i++ )
        client_sockets_FD[i] = 0;
    //create master socket
    if( master_socket = socket(AF_INET, SOCK_STREAM, 0) <= 0 ) {
        perror("socket failed: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd address bind reusable
    if ( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <= 0 ) {
        perror("setsockopt SO_REUSEADDR err:");
        exit(EXIT_FAILURE);
    }
    // make master_fd non_blocking
    set_non_blocking_socket(master_socket);
}


void bind_and_listen(void) {
    if(bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind err:");
        exit(EXIT_FAILURE);
    }
    printf("bind successfully\n");
    if(listen(master_socket, 10) < 0) {
        perror("bind err:");
        exit(EXIT_FAILURE);
    }
    printf("listening successfully\n");
}

void build_read_set(void) {
    //initialize read_fds with 0s
    FD_ZERO(&read_fds);
    //set master_FD
    FD_SET(master_socket, &read_fds);
    highest_fd_val = master_socket;
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        FD_SET(client_sockets_FD[i], &read_fds);
        if(client_sockets_FD[i] > highest_fd_val)
            highest_fd_val = client_sockets_FD[i];
    }
}

void select_accept_recv_send_handler(void) {
    int select_fds = 0;
    while (TRUE)
    {
        build_read_set();
        select_fds = select(highest_fd_val + 1, &read_fds, NULL, NULL, NULL);
        //accept-recv-send
    }
    
}

void initialize_server_address(void) {
    address.sin_family = AF_INET;
    address.sin_port = PORT;
    address.sin_addr.s_addr = INADDR_ANY;
}


int main() {
    return 0;
}