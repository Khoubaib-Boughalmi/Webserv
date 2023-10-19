#include "server.hpp"

Server::Server(char *config_file)
{
    std::vector<std::string> servers;
    int servers_nb = 0;
    std::string conf;
    read_file(config_file, conf, servers_nb);
    parse_file(conf, servers_nb, servers);
    Server::initialization_and_socket_creation(servers, servers.size());
    Server::bind_and_listen();
    Server::select_accept_recv_send_handler();

}

Server::~Server() {}

Server::Server(const Server &copy)
{
    *this = copy;
}

Server &Server::operator=(const Server &copy)
{
    if (this != &copy)
    {
        this->opt = copy.opt;
        this->addrlen = copy.addrlen;
        master_sockets = copy.master_sockets;
        this->highest_fd_val = copy.highest_fd_val;
        this->sockets_FD = copy.sockets_FD;
        this->read_fds = copy.read_fds;
        this->write_fds = copy.write_fds;
        this->master_fds = copy.master_fds;
        this->addresses = copy.addresses;
    }
    return *this;
}

void Server::cleanFDSet(void)
{
    FD_ZERO(&(this->master_fds));
    FD_ZERO(&(this->read_fds));
    FD_ZERO(&(this->write_fds));
}

void Server::add_fd_to_master_set(int fd, Servers &server)
{
    // Client *new_client = new Client(fd, master_sockets[0].GetSock(), server); // serverFD not important for now
    Client client(fd, master_sockets[0].GetSock(), server);
    sockets_FD.push_back(client);
    
    FD_SET(fd, &(this->master_fds));
    if (fd > this->highest_fd_val)
        this->highest_fd_val = fd;
}

void Server::set_non_blocking_socket(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw Servers::ServerException("fcntl err: ");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw Servers::ServerException("fcntl err: ");
}

void Server::initialize_server_address(const char *ip)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = inet_addr(ip);
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(std::strtol(ip, NULL, 10)); //
    addrlen = sizeof(address);
    this->addresses.push_back(address);
}

void Server::initialization_and_socket_creation(std::vector<std::string> &servers, size_t servers_nb)
{
    // initialize this->sockets_FD
    this->opt = 1;
    //
    std::vector<std::string>::iterator it = servers.begin();
    Servers tmp;
    Server::cleanFDSet();
    // create master socket
    //  Number of servers already known
    //  master_sockets.reserve(hosts_n);
    for (size_t i = 0; i < servers_nb; i++)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        //
        // Servers tmp(*it) // throws BadSocketException
        CreateServer(*it, sock, tmp, i);

        // tmp.print();
        master_sockets.push_back(tmp);
        if (master_sockets[i].GetSock() < 0)
        {
            throw Servers::ServerException("Master socket creation err: ");
        }
        // make master_fd address reusable
        if (setsockopt(master_sockets[i].GetSock(), SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
        {
            throw Servers::ServerException("setsockopt err: ");
        }
        // make master_fd non_blocking&add it to master_fds
        set_non_blocking_socket(master_sockets[i].GetSock());
        add_fd_to_master_set(master_sockets[i].GetSock(), tmp);
        // system("leaks webserv");
        // exit(EXIT_SUCCESS);
        initialize_server_address(std::to_string(tmp.GetPort()).c_str());
        std::cout << "Server is listening on port " << tmp.GetPort() << std::endl;
        if (it != servers.end())
        {
            ++it;
            tmp.clear();
        }
    }
}

void Server::bind_and_listen(void)
{
    // bind master_fd to address
    for (size_t i = 0; i < master_sockets.size(); i++)
    {
        if (bind(master_sockets[i].GetSock(), (struct sockaddr *)&(this->addresses[i]), sizeof(this->addresses[i])) < 0)
        {
            throw Servers::ServerException("Bind err: ");
        }
        // listen to master_fd
        if (listen(master_sockets[i].GetSock(), 10) < 0)
        {
            throw Servers::ServerException("Listen err: ");
        }
    }
}

void Server::update_client_connected_time(int fd)
{
    for (unsigned int index = 0; index < sockets_FD.size(); index++)
    {
        if (sockets_FD[index].clientFD == fd)
        {
            sockets_FD[index].update_connected_time();
            break;
        }
    }
}

void Server::init_read_write_fd_set(void)
{
    // reinitialize master_fds because select() modifies it
    cleanFDSet();
    for (unsigned int i = 0; i < sockets_FD.size(); i++)
        FD_SET(sockets_FD[i].clientFD, &(this->master_fds));
    read_fds = master_fds;
}

void Server::accept_new_request(Servers &active_server)
{
    // FD_CLR(master_sockets[0], &(this->read_fds));
    int client_fd = accept(active_server.GetSock(), (struct sockaddr *)&(this->addresses[0]), &(this->addrlen));
    if (client_fd < 0)
    {
        throw Servers::ServerException("Accept err: ");
        return ;                            //
    }
    std::cout << "New request accepted" << std::endl;
    set_non_blocking_socket(client_fd);
    add_fd_to_master_set(client_fd, active_server);
    update_client_connected_time(client_fd);
    // update the connected time
    // no available slots in this->sockets_FD
    if (sockets_FD.size() >= MAX_CONNECTIONS)
        std::cout << "Server is overloaded try later.." << std::endl; // should be sent to client
}


int get_numberof(std::string str, std::string substr)
{
    int number = 0;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        number++;
        pos += substr.length();
    }
    return number;
}


int check_the_req(std::string req)
{
    std::string method = req.substr(0, req.find(" "));
    std::cout << "method " << method << std::endl;

    int n_newlines = get_numberof(req, "\r\n\r\n");
    std::cout << "number of it " << n_newlines << std::endl;
    int FORPOSTSHOUDBE = 1;
    if(method=="POST")
    {
        // find boundary
        size_t boundarypos = req.find("boundary=");
        if(boundarypos != std::string::npos)
        {
            std::string boundary = req.substr(boundarypos+9, req.find("\r\n", boundarypos)-boundarypos-9);
            FORPOSTSHOUDBE=get_numberof(req,boundary)-1;
        }
        else{
            //find chunked in headers
            std::string headers=req.substr(0, req.find("\r\n\r\n"));
            if(headers.empty())
                return 0;
            size_t chunkedpos = headers.find("chunked");
            if(chunkedpos != std::string::npos)
            {
                FORPOSTSHOUDBE = 2;
            }
        }
    }
    std::cout << "n_newlines " << n_newlines << std::endl;

    if(((method=="GET" ||  method =="DELETE"||  method =="HEAD" ) && n_newlines == 1 ) || (method=="POST" && n_newlines == FORPOSTSHOUDBE) || (method!="GET" && method!="POST" && method!="DELETE" && method!="HEAD"))
    {
        return 1;
    }


    return 0;
}

int Server::receive(int fd)
{
    int req;
    char buff[700000] = {0};
    std::string final_req;
    memset(buff, 0, sizeof(buff));
    while(true)
    {
        req = recv(fd, buff, sizeof(buff), 0);
        if (req == 0)
            break;
        if(req < 0)
        {
            if(check_the_req(final_req))
                break;
            continue;                
        }
        final_req += std::string(buff, req);
        memset(buff, 0, sizeof(buff));
    }


    // client disconnected
    if (!req)
    {
        std::cout << "Client with fd " << fd << " Disconnected" << std::endl;
        close(fd);
        FD_CLR(fd, &(this->read_fds));
        FD_CLR(fd, &(this->master_fds));
        for (unsigned int index = 0; index < sockets_FD.size(); index++)
        {
            if (sockets_FD[index].clientFD == fd)
            {
                // delete &sockets_FD[index]; //CHECK THIS FOR ERRS
                sockets_FD.erase(sockets_FD.begin() + index); // remove mutant fd from sockets_FD
                return 0;
            }
        }
    }
    FD_SET(fd, &(this->write_fds));
    update_client_connected_time(fd);
    for (unsigned int index = 0; index < sockets_FD.size(); index++)
    {
        if (sockets_FD[index].clientFD == fd)
        {
            std::cout << sockets_FD[index].server.GetPort() << std::endl;
            // std::cout << "================"<< std::endl;
            // std::cout << "Request: " << final_req << std::endl;
            // std::cout << "@@@@@@@@@@@@@@@@@@@@@@@"<< std::endl;
            sockets_FD[index].clientRequest = Request(final_req);
        }
    }
    return (1);
}

void Server::send(Client *clientInfo)
{
    Request req = clientInfo->clientRequest;
    parse_request(req, clientInfo);
}

int Server::check_if_fd_is_server(int fd)
{
    for (size_t i = 0; i < master_sockets.size(); i++)
    {
        if (fd == master_sockets[i].GetSock())
            return 1;
    }
    return (0);
}

void Server::check_for_timeout(void)
{
    for (size_t index = 0; index < sockets_FD.size(); index++)
    {
        double elapsedTime = (time(0) - sockets_FD[index].connected_time);
        if (!check_if_fd_is_server(sockets_FD[index].clientFD) && elapsedTime > 5)
        {
            std::cout << "Client with fd " << sockets_FD[index].clientFD << " Timed out" << std::endl;
            close(sockets_FD[index].clientFD);
            FD_CLR(sockets_FD[index].clientFD, &(this->read_fds));
            FD_CLR(sockets_FD[index].clientFD, &(this->master_fds));
            // delete &sockets_FD[index];
            sockets_FD.erase(sockets_FD.begin() + index); // remove client fd from sockets_FD after sending response
            // exit(EXIT_FAILURE);
        }
    }
}

void Server::handle_already_existing_connection(void)
{
    // loop through this->read_fds if any slot is set then it's a client request
    for (size_t index = 0; index < sockets_FD.size(); index++)
    {
        if (FD_ISSET(sockets_FD[index].clientFD, &(this->read_fds)) && !check_if_fd_is_server(sockets_FD[index].clientFD))
        {
            if (!receive(sockets_FD[index].clientFD))
            {
                break;
            }
        }
        if (FD_ISSET(sockets_FD[index].clientFD, &(this->write_fds)) && !check_if_fd_is_server(sockets_FD[index].clientFD))
        {
            send(&sockets_FD[index]);
            break;
        }
    }
}

Servers Server::check_ISSET_master_fds()
{
    for (size_t i = 0; i < master_sockets.size(); i++)
    {
        if (FD_ISSET(master_sockets[i].GetSock(), &(this->read_fds)))
            return master_sockets[i];
    }
    return Servers();
}

void Server::select_accept_recv_send_handler(void)
{
    int activity_fds;
    struct timeval tv;
    Servers active_server;
    while (TRUE)
    {
        init_read_write_fd_set();
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        activity_fds = select(1024, &(this->read_fds), &(this->write_fds), NULL, &tv);
        if (activity_fds < 0)
        {
            throw Servers::ServerException("select err: ");
        }
        check_for_timeout();
        // if there is an activty in master_sockets[0] then it's a new request
        active_server = check_ISSET_master_fds();
        if (!(active_server.getHost().empty()))
        {
            accept_new_request(active_server);
        }
        else
        { // connection is already established
            handle_already_existing_connection();
        }
    }
}