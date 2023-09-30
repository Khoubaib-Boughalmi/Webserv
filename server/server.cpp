#include "server.hpp"

Server::Server(/* args */)
{
    Server::initialization_and_socket_creation();
    Server::initialize_server_address();
    Server::bind_and_listen();
    Server::select_accept_recv_send_handler();
}

Server::~Server()
{
}

void Server::set_non_blocking_socket(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl err: "); // to be changed to log
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl err: ");
        exit(EXIT_FAILURE);
    }
}

void Server::initialization_and_socket_creation (void) {
    //initialize this->client_sockets_FD
    this->opt = 1;
    for ( int i = 0; i < MAX_CONNECTIONS; i++ )
        this->client_sockets_FD[i] = 0;
    //create master socket
    this->master_socket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Master sock: " << this->master_socket << std::endl; 
    if (this->master_socket < 0) {
        perror("Master socket creation err: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd address bind reusable
    if (setsockopt(this->master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt err: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd non_blocking
    set_non_blocking_socket(this->master_socket);
}


void Server::bind_and_listen(void) {
    //bind master_fd to address
    if (bind(this->master_socket, (struct sockaddr *)&(this->address), sizeof(this->address)) < 0) {
        perror("Bind err: ");
        exit(EXIT_FAILURE);
    }
    //listen to master_fd
    if (listen(this->master_socket, 3) < 0) {
        perror("Listen err: ");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening on port: " << PORT << std::endl; 
}

void Server::build_read_set(void) {
    FD_ZERO(&(this->read_fds));
    FD_SET(this->master_socket, &(this->read_fds));
    this->highest_fd_val = this->master_socket;
    for (int index = 0; index < MAX_CONNECTIONS; index++)
    {
        if(this->client_sockets_FD[index] > 0)
            FD_SET(this->client_sockets_FD[index], &(this->read_fds));
        if(this->client_sockets_FD[index] > this->highest_fd_val)
            this->highest_fd_val = this->client_sockets_FD[index];
    }
}

void Server::accept_new_request(void) {
    int client_fd = accept(this->master_socket, (struct sockaddr *) &(this->address), &(this->addrlen));
    if(client_fd < 0)
    {
        perror("Client Connection err: ");
        exit(EXIT_FAILURE);
    }
    //add new clients fd to this->client_sockets_FD
    int index = 0;
    for (; index < MAX_CONNECTIONS; index++)
    {
        //find an empty slot
        if(this->client_sockets_FD[index] == 0) {
            std::cout << "New request accepted" << std::endl;
            this->client_sockets_FD[index] = client_fd;
            break;
        }
    }
    //no available slots in this->client_sockets_FD
    if(index == MAX_CONNECTIONS)
        std::cout << "Server is overloaded try later.." << std::endl; //should be sent to client
}

void Server::select_accept_recv_send_handler(void) {
    int     activity_fds;
    char    buff[1024];
    int     req;
    std::string response = "HTTP/1.1 200 OK\r\nDate: Sat, 24 Sep 2023 12:00:00 GMT\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Sample Page</title>\r\n<style>body {background-color: #f0f0f0;margin: 0;padding: 0;}h1 {color: blue;}p {color: red;}</style>\r\n</head>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n<p>This is a sample page.</p>\r\n</body>\r\n</html>\r\n";
    
    while (TRUE)
    {
        build_read_set();
        activity_fds = select(1024, &(this->read_fds), NULL, NULL, NULL); 
        if (activity_fds < 0){
            perror("select err: ");
            exit(EXIT_FAILURE);
        }
        if (activity_fds == 0)
            continue;
        //if there is an activty in this->master_socket then it's a new request
        if (FD_ISSET(this->master_socket, &(this->read_fds))) {
            accept_new_request();
        }
        //loop through this->read_fds if any slot is set then it's a client request
        for (int index = 0; index < MAX_CONNECTIONS; index++)
        {
            if(FD_ISSET(this->client_sockets_FD[index], &(this->read_fds)) && this->client_sockets_FD[index] != this->master_socket ) {
                req = recv(this->client_sockets_FD[index], &buff, sizeof(buff), 0);
                //client disconnected
                if(!req) {
                    std::cout << "Client with fd " << this->client_sockets_FD[index] << " Disconnected" << std::endl;
                    close(this->client_sockets_FD[index]);  
                    this->client_sockets_FD[index] = 0;  
                    continue;
                }
                std::cout << "Request from client with fd: " << this->client_sockets_FD[index] << std::endl;
                if(send(this->client_sockets_FD[index], response.c_str(), response.length() , 0) < 0){
                    perror("Send err: ");
                    exit(EXIT_FAILURE);
                }
                close(this->client_sockets_FD[index]);
                this->client_sockets_FD[index] = 0;
            }
        }    
    }
}

void Server::initialize_server_address(void) {
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(PORT);
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->addrlen = sizeof(this->address);
}
