#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h> 

#define TRUE            1
#define FALSE           0
#define MAX_CONNECTIONS 42
#define PORT            8080

class Server
{
    private:
        /* data */
    public:
    int         opt;
    socklen_t   addrlen;
    int         master_socket;
    int         highest_fd_val;
    int         client_sockets_FD[MAX_CONNECTIONS];

    fd_set              read_fds;
    struct sockaddr_in  address;  
    //default constructor copy constructor and assignment operator are deleted    
    Server();
    Server(const Server &copy);
    Server &operator=(const Server &copy);
    ~Server();
    void initialization_and_socket_creation (void);
    void accept_new_request (void);
    void initialize_server_address (void);
    void bind_and_listen (void);
    void build_read_set (void);
    void set_non_blocking_socket (int fd);
    void select_accept_recv_send_handler (void);

};

#endif