#include "server.hpp"

Server::Server(/* args */)
{
    Server::initialization_and_socket_creation();
    Server::bind_and_listen();
    Server::select_accept_recv_send_handler();
}

Server::~Server()
{
}

Server::Server(const Server &copy) {
    *this = copy;
}

Server &Server::operator=(const Server &copy) {
    if (this != &copy) {
        this->opt = copy.opt;
        this->addrlen = copy.addrlen;
        this->master_socket = copy.master_socket;
        this->highest_fd_val = copy.highest_fd_val;
        this->sockets_FD = copy.sockets_FD;
        this->read_fds = copy.read_fds;
        this->write_fds = copy.write_fds;
        this->master_fds = copy.master_fds;
        this->address = copy.address;
    }
    return *this;
}

void Server::cleanFDSet(void) {
    FD_ZERO(&(this->master_fds));
    FD_ZERO(&(this->read_fds));
    FD_ZERO(&(this->write_fds));
}

void Server::add_fd_to_master_set(int fd) {
    Client *new_client = new Client(fd, this->master_socket);
    sockets_FD.push_back(*new_client);
    FD_SET(fd, &(this->master_fds));
    if(fd > this->highest_fd_val)
        this->highest_fd_val = fd;
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

void Server::initialize_server_address(void) {
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(PORT);
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->addrlen = sizeof(this->address);
}

void Server::initialization_and_socket_creation (void) {
    //initialize this->sockets_FD
    this->opt = 1;
    this->master_socket = 0;

    Server::cleanFDSet();
    //create master socket
    this->master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->master_socket < 0) {
        perror("Master socket creation err: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd address reusable
    if (setsockopt(this->master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt err: ");
        exit(EXIT_FAILURE);
    }
    // make master_fd non_blocking&add it to master_fds
    set_non_blocking_socket(this->master_socket);
    add_fd_to_master_set(this->master_socket);
    //initialize server address struct sockaddr_in
    initialize_server_address();
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

void Server::update_client_connected_time(int fd) {
    for (unsigned int index = 0; index < sockets_FD.size(); index++){
        if(sockets_FD[index].clientFD == fd) {
            sockets_FD[index].update_connected_time();
            break;
        }
    }
}

void Server::init_read_write_fd_set(void) {
    //reinitialize master_fds because select() modifies it
    cleanFDSet();
    for(unsigned int i = 0; i < sockets_FD.size(); i++)
        FD_SET(sockets_FD[i].clientFD , &(this->master_fds));
    read_fds = master_fds;
}

void Server::accept_new_request(void) {
    // FD_CLR(this->master_socket, &(this->read_fds));
    int client_fd = accept(this->master_socket, (struct sockaddr *) &(this->address), &(this->addrlen));
    if(client_fd < 0)
    {
        perror("Client Connection err: ");
        return ;
    }
    std::cout << "New request accepted" << std::endl;
    set_non_blocking_socket(client_fd);
    add_fd_to_master_set(client_fd);
    update_client_connected_time(client_fd);
        //update the connected time
    //no available slots in this->sockets_FD
    if(sockets_FD.size() >= MAX_CONNECTIONS)
        std::cout << "Server is overloaded try later.." << std::endl; //should be sent to client
}

int Server::receive(int fd) {
    int req;
    char buff[1024] = {0};

    // FD_CLR(fd, &(this->read_fds));
    std::cout << "Receiving request from client with fd: " << fd << std::endl;
    req = recv(fd, &buff, sizeof(buff), 0);
    //client disconnected
    if(!req) {
        std::cout << "Client with fd " << fd << " Disconnected" << std::endl;
        close(fd);
        FD_CLR(fd, &(this->read_fds));
        FD_CLR(fd, &(this->master_fds));
        for (unsigned int index = 0; index < sockets_FD.size(); index++){
            if(sockets_FD[index].clientFD == fd) {
                // delete &sockets_FD[index]; //CHECK THIS FOR ERRS
                sockets_FD.erase(sockets_FD.begin() + index); //remove mutant fd from sockets_FD
                return 0;
            }
        }
    }
    std::cout << "Request from client with fd: " << fd << std::endl;
    FD_SET(fd, &(this->write_fds));
    update_client_connected_time(fd);
    printf("Request: %s\n", buff);
    return 1;
}

void Server::send(int fd, int index) {
    int valread;
    size_t total = 0;
    std::string response = "HTTP/1.1 200 OK\r\nDate: Sat, 24 Sep 2023 12:00:00 GMT\r\nContent-Type: text/html\r\nContent-Length: 350\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Sample Page</title>\r\n<style>body {background-color: #f0f0f0;margin: 0;padding: 0;}h1 {color: blue;}p {color: red;}</style>\r\n</head>\r\n<body>\r\n<h1>Hello world!</h1>\r\n<p>This is a sample page.</p>\r\n<img src=\"https://images.pexels.com/photos/842711/pexels-photo-842711.jpeg\" alt=\"W3Schools.com\">\r\n</body>\r\n</html>\r\n";
    size_t bytesleft = response.length();

    //send may not send all bytes at once so we loop until all bytes are sent
    while (total < response.length())
    {
        if((valread = ::send(fd, response.c_str() + total, bytesleft , 0)) < 0){
                perror("Send err: ");
                exit(EXIT_FAILURE);
        }
        if (valread == -1) { break; }
        total += valread;
        bytesleft -= valread;
    }

    // delete &sockets_FD[index];
    (void)index;
    // close(fd);
    // sockets_FD.erase(sockets_FD.begin() + index); //remove client fd from sockets_FD after sending response
}

void Server::check_for_timeout(void) {
    for (size_t index = 0; index < sockets_FD.size() ; index++)
    {
        double elapsedTime = (time(0) - sockets_FD[index].connected_time);
        if (sockets_FD[index].clientFD != master_socket && elapsedTime > 10) {
            std::cout << "Client with fd " << sockets_FD[index].clientFD << " Timed out" << std::endl;
            close(sockets_FD[index].clientFD);
            FD_CLR(sockets_FD[index].clientFD, &(this->read_fds));
            FD_CLR(sockets_FD[index].clientFD, &(this->master_fds));
            // delete &sockets_FD[index];
            sockets_FD.erase(sockets_FD.begin() + index); //remove client fd from sockets_FD after sending response
            // exit(EXIT_FAILURE);
        }        
        
    }
}

void Server::handle_already_existing_connection(void) {
    //loop through this->read_fds if any slot is set then it's a client request
    for (size_t index = 0; index < sockets_FD.size(); index++)
    {
        if(FD_ISSET(sockets_FD[index].clientFD, &(this->read_fds)) && sockets_FD[index].clientFD != this->master_socket ) {
            std::cout << "Connection FD" << sockets_FD[index].clientFD << std::endl;

            if(!receive(sockets_FD[index].clientFD)) {
                break;
            }
        }
        if(FD_ISSET(sockets_FD[index].clientFD, &(this->write_fds)) && sockets_FD[index].clientFD != this->master_socket ) {
            send(sockets_FD[index].clientFD, index);
            break;
        }
    }
}

void Server::select_accept_recv_send_handler(void) {
    int     activity_fds;
    struct timeval tv;
    
    while (TRUE)
    {
        init_read_write_fd_set();
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        activity_fds = select(highest_fd_val + 1, &(this->read_fds), &(this->write_fds), NULL, &tv); 
        if (activity_fds < 0){
            perror("select err: ");
            exit(EXIT_FAILURE);
        }
        check_for_timeout();
        //if there is an activty in this->master_socket then it's a new request
        if (FD_ISSET(this->master_socket, &(this->read_fds))) {
            std::cout << "New Connection" << std::endl;
            accept_new_request();
        }
        else { //connection is already established
            handle_already_existing_connection();
        }
    }
}


