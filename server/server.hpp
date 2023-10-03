#ifndef SERVER_HPP
#define SERVER_HPP

#include "client.hpp"

#define TRUE            1
#define FALSE           0
#define MAX_CONNECTIONS 42
#define PORT            8080


class Client;
class Server
{
    private:
        /* data */
    public:
    int             opt;
    socklen_t       addrlen;
    int             master_socket;
    int             highest_fd_val;
    std::vector<Client>  sockets_FD;

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
    int receive (int fd);
    void send (int fd, int index);
    void cleanFDSet (void);
    void check_for_timeout (void);
    void update_client_connected_time (int fd);
};

#endif