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

void set_non_blocking_socket(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl err: ");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl err: ");
        exit(EXIT_FAILURE);
    }
}

void initialization_and_socket_creation (void) {
    //initialize client_sockets_FD
    for ( int i = 0; i < MAX_CONNECTIONS; i++ )
        client_sockets_FD[i] = 0;
    //create master socket
    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    printf("Master sock: %d\n", master_socket);
    if (master_socket < 0) {
        perror("Master socket creation err: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd address bind reusable
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt err: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd non_blocking
    set_non_blocking_socket(master_socket);
}


void bind_and_listen(void) {
    //bind master_fd to address
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind err: ");
        exit(EXIT_FAILURE);
    }
    //listen to master_fd
    if (listen(master_socket, 3) < 0) {
        perror("Listen err: ");
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d \n", PORT);
}

void build_read_set(void) {
    FD_ZERO(&read_fds);
    FD_SET(master_socket, &read_fds);
    highest_fd_val = master_socket;
    for (int index = 0; index < MAX_CONNECTIONS; index++)
    {
        if(client_sockets_FD[index] > 0)
            FD_SET(client_sockets_FD[index], &read_fds);
        if(client_sockets_FD[index] > highest_fd_val)
            highest_fd_val = client_sockets_FD[index];
    }
}

void accept_new_request(void) {
    int client_fd = accept(master_socket, (struct sockaddr *) &address, &addrlen);
    if(client_fd < 0)
    {
        perror("Client Connection err: ");
        exit(EXIT_FAILURE);
    }
    //add new clients fd to client_sockets_FD
    int index = 0;
    for (; index < MAX_CONNECTIONS; index++)
    {
        //find an empty slot
        if(client_sockets_FD[index] == 0) {
            printf("New request accepted\n");
            client_sockets_FD[index] = client_fd;
            break;
        }
    }
    //no available slots in client_sockets_FD
    if(index == MAX_CONNECTIONS)
        printf("Server is overloaded try later..\n"); //should be sent to client
}

void select_accept_recv_send_handler(void) {
    int     activity_fds;
    char    buff[1024];
    int     req;
    char    *response = "HTTP/1.1 200 OK\r\nDate: Sat, 24 Sep 2023 12:00:00 GMT\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Sample Page</title>\r\n<style>body {background-color: #f0f0f0;margin: 0;padding: 0;}h1 {color: blue;}p {color: red;}</style>\r\n</head>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n<p>This is a sample page.</p>\r\n</body>\r\n</html>\r\n";
    
    while (TRUE)
    {
        build_read_set();
        activity_fds = select(1024, &read_fds, NULL, NULL, NULL);        
        printf("activity_fds: %d\n", activity_fds);
        if (activity_fds < 0){
            perror("select err: ");
            exit(EXIT_FAILURE);
        }
        if (activity_fds == 0)
            continue;
        //if there is an activty in master_socket then it's a new request
        if (FD_ISSET(master_socket, &read_fds)) {
            accept_new_request();
        }
        //loop through read_fds if any slot is set then it's a client request
        for (int index = 0; index < MAX_CONNECTIONS; index++)
        {
            if(FD_ISSET(client_sockets_FD[index], &read_fds) && \
            client_sockets_FD[index] != master_socket ) {
                memset(buff, 0, sizeof(buff));
                req = recv(client_sockets_FD[index], &buff, sizeof(buff), 0);
                //client disconnected
                if(!req) {
                    printf("Client with fd %d Disconnected\n", client_sockets_FD[index]);
                    close(client_sockets_FD[index]);  
                    client_sockets_FD[index] = 0;  
                    continue;
                }
                printf("New request Socket with fd: %d", client_sockets_FD[index]);
                if(send(client_sockets_FD[index], response, strlen(response), 0) < 0){
                    perror("Send err: ");
                    exit(EXIT_FAILURE);
                }
                close(client_sockets_FD[index]);
                client_sockets_FD[index] = 0;
            }
        }
         
    }
    
}

void initialize_server_address(void) {
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;
    addrlen = sizeof(address);
}


int main() {
    initialization_and_socket_creation();
    initialize_server_address();
    bind_and_listen();
    select_accept_recv_send_handler();
    return 0;
}