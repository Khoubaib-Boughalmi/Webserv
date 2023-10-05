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
    int                         opt;
    socklen_t                   addrlen;
    std::vector<int>            master_sockets;
    // int            master_socket;
    int                         highest_fd_val;
    std::vector<Client>         sockets_FD;

    fd_set                      read_fds;
    fd_set                      write_fds;
    fd_set                      master_fds;
    std::vector<struct sockaddr_in>          addresses;  
    std::vector<std::string>    ips;

    //default constructor copy constructor and assignment operator are deleted    
    Server();
    Server(const Server &copy);
    Server &operator=(const Server &copy);
    ~Server();
    void initialization_and_socket_creation (void);
    void accept_new_request (int active_server);
    void initialize_server_address (const char *);
    void bind_and_listen (void);
    void init_read_write_fd_set (void);
    void set_non_blocking_socket (int fd);
    void select_accept_recv_send_handler (void);
    void add_fd_to_master_set (int fd);
    void handle_already_existing_connection(void) ;
    int receive (int fd);
    void send (int fd, int index);
    void cleanFDSet (void);
    void check_for_timeout (void);
    void update_client_connected_time (int fd);
    int check_ISSET_master_fds(void);
    int check_if_fd_is_server(int fd);
};

#endif