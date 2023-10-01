#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
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
    int             opt;
    socklen_t       addrlen;
    int             master_socket;
    int             highest_fd_val;
    std::vector<int>  sockets_FD;

    fd_set              read_fds;
    fd_set              write_fds;
    fd_set              master_fds;
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
    void init_read_write_fd_set (void);
    void set_non_blocking_socket (int fd);
    void select_accept_recv_send_handler (void);
    void add_fd_to_master_set (int fd);
    void receive (int fd);
    void send (int fd, int index);

};

#endif