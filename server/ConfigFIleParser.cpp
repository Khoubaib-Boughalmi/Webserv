# include "ConfigFileParser.hpp"

ServerData::ServerData() {}

void   ServerData::SetHost(const std::string server_host) { host = server_host; }
const std::string& ServerData::getHost() const { return host; }

void    ServerData::SetPort(int server_port) { port = server_port; }
const int& ServerData::GetPort() const { return port; }

void    ServerData::SetDefault(bool default_server) { is_default = default_server; }
const bool&    ServerData::GetDefault() const { return is_default; }

void    ServerData::SetServerNames(std::vector<std::string>& s_names) {
    std::vector<std::string>::iterator  it = s_names.begin();
    while (it != s_names.end()) {
        serverNames.push_back(*it);
        ++it;
    }
}
const std::vector<std::string>& ServerData::getServerNames() const { return serverNames; }


void    ServerData::SetErrorPages(std::vector<std::string>& err_pages) {
     std::vector<std::string>::iterator  it = err_pages.begin();
    while (it != err_pages.end()) {
        error_pages.push_back(*it);
        ++it;
    }
}
const std::vector<std::string>& ServerData::GetErrorPages() const { return error_pages; }

void    ServerData::SetClientBodyLimit(const std::string& bodyLimit) { clientBodyLimit = bodyLimit; }
const std::string&  ServerData::GetClientBodyLimit() const { return clientBodyLimit; }


void    ServerData::SetRoutes(Routes& route) {
    routes.push_back(route);
}
std::vector<Routes>    ServerData::GetRoutes() const { return routes; }

void    ServerData::print() {
    std::cout << "\x1b[1;33m╔═════════════════════════════════════════╗\x1b[0m\n";
    std::cout << "\x1b[1;33m║          ServerData Configuration       ║\x1b[0m\n";
    std::cout << "\x1b[1;33m╠═════════════════════════════════════════╣\x1b[0m\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mHost               : \x1b[0m" << host << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mPort               : \x1b[0m" << port << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mDefault Server     : \x1b[0m" << (is_default ? "\x1b[1;32mtrue\x1b[0m" : "\x1b[1;31mfalse\x1b[0m") << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mClient Body Limit  : \x1b[0m" << clientBodyLimit << "\n";
    
    int x = 1;
    {
        std::vector<Routes>::const_iterator rit = routes.begin();
        while (rit != routes.end()) {
            std::cout << "\x1b[1;33m║ ----Route " << x++ << "-------\x1b[0m\n";
            Routes tmp = *rit;
            tmp.print();
            ++rit;
        }
    }
    
    std::cout << "\x1b[1;33m╚═════════════════════════════════════════╝\x1b[0m\n";
}

void    ServerData::clear() {
    host.clear();
    port = 0;
    is_default = false;
    serverNames.clear();
    error_pages.clear();
    clientBodyLimit.clear();
    routes.clear();
}

ServerData::~ServerData() {}

//
Routes::Routes() {}

void    Routes::SetPath(const std::string& pth) { path = pth; }

void    Routes::SetMethods(std::vector<std::string>& m) { 
    std::vector<std::string>::iterator  it = m.begin();
    while (it != m.end()) {
        methods.push_back(*it);
        ++it;
    }
}

void    Routes::SetDirectoryListing(bool d_lst) { directory_listing = d_lst; }

void    Routes::SetDefaultFile(const std::string& d_file) { default_file = d_file; } 

void    Routes::SetCgiExtension(const std::string& cgi_ext) { cgi_extensions = cgi_ext; }

void    Routes::SetCgiBin(const std::string& cgi_b) { cgi_bin = cgi_b; }

void    Routes::SetUploadEnabled(bool up_en) { upload_enabled = up_en; }

void    Routes::SetUploadDirectory(const std::string& up_dir) { upload_directory = up_dir; }

void    Routes::SetRedirectUrl(const std::string& r_url) { redirect_url = r_url; }

void    Routes::clear() {
    path.clear();
    methods.clear();
    directory_listing = false;
    default_file.clear();
    cgi_extensions.clear();
    cgi_bin.clear();
    upload_enabled = false;
    upload_directory.clear();
    redirect_url.clear();
}

void    Routes::print() {
    std::cout << "\x1b[1;33m║ \x1b[1;34mPath        : \x1b[0m" << path << "\n";

    std::cout << "\x1b[1;33m║ \x1b[1;34mMethods     : \x1b[0m";
    std::vector<std::string>::iterator it;
    it = methods.begin();
    while (it != methods.end()) {
        std::cout << *it;
        ++it;
        if (it != methods.end()) {
            std::cout << "\t";
        }
    }
    std::cout << "\n";
    
    std::cout << "\x1b[1;33m║ \x1b[1;34mDirectory Listing : \x1b[0m" << (directory_listing ? "\x1b[1;32mtrue\x1b[0m" : "\x1b[1;31mfalse\x1b[0m") << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mDefault File : \x1b[0m" << default_file << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mCGI Extensions : \x1b[0m" << cgi_extensions << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mCGI Bin : \x1b[0m" << cgi_bin << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mUpload Enabled : \x1b[0m" << (upload_enabled ? "\x1b[1;32mtrue\x1b[0m" : "\x1b[1;31mfalse\x1b[0m") << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mUpload Directory : \x1b[0m" << upload_directory << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mRedirect URL : \x1b[0m" << redirect_url << "\n";
}

Routes::~Routes() {}
//
// TODO: remove all comments from config file

//
void ServerData::add_fd_to_master_set(int fd) {
    sockets_FD.push_back(fd);
    std::sort(sockets_FD.begin(), sockets_FD.end());
    FD_SET(fd, &(this->master_fds));
    if(fd > this->highest_fd_val)
        this->highest_fd_val = fd;
}

void ServerData::set_non_blocking_socket(int fd) {
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

void ServerData::initialize_server_address(void) {
    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(port);
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->addrlen = sizeof(this->address);
}

void ServerData::initialization_and_socket_creation (void) {
    //initialize this->sockets_FD
    this->opt = 1;
    this->master_socket = 0;

    FD_ZERO(&(this->master_fds));
    FD_ZERO(&(this->read_fds));
    FD_ZERO(&(this->write_fds));
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

void ServerData::bind_and_listen(void) {
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
    std::cout << "Listening on port: " << port << std::endl; 
}

void ServerData::init_read_write_fd_set(void) {
    // FD_ZERO(&(this->read_fds));
    // FD_ZERO(&(this->write_fds));
    read_fds = master_fds;
    write_fds = master_fds;
    
}

void ServerData::accept_new_request(void) {
    FD_CLR(this->master_socket, &(this->read_fds));
    int client_fd = accept(this->master_socket, (struct sockaddr *) &(this->address), &(this->addrlen));
    if(client_fd < 0)
    {
        perror("Client Connection err: ");
        return ;
    }
    std::cout << "New request accepted" << std::endl;
    set_non_blocking_socket(client_fd);
    add_fd_to_master_set(client_fd);
    //no available slots in this->sockets_FD
    if(sockets_FD.size() >= MAX_CONNECTIONS)
        std::cout << "ServerData is overloaded try later.." << std::endl; //should be sent to client
}

void ServerData::receive(int fd) {
    int req;
    char buff[1024] = {0};

    FD_CLR(fd, &(this->read_fds));
    req = recv(fd, &buff, sizeof(buff), 0);
    //client disconnected
    if(!req) {
        std::cout << "Client with fd " << fd << " Disconnected" << std::endl;
        close(fd);  
    }
    std::cout << "Request from client with fd: " << fd << std::endl;
    std::cout << buff << std::endl;
    // printf("Request: %s\n", buff);
}

void ServerData::select_accept_recv_send_handler(void) {
    int     activity_fds;
    std::string response = "HTTP/1.1 200 OK\r\nDate: Sat, 24 Sep 2023 12:00:00 GMT\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Sample Page</title>\r\n<style>body {background-color: #f0f0f0;margin: 0;padding: 0;}h1 {color: blue;}p {color: red;}</style>\r\n</head>\r\n<body>\r\n<h1>Boobies!</h1>\r\n<p>This is a sample page.</p>\r\n</body>\r\n</html>\r\n";
    
    while (TRUE)
    {
        init_read_write_fd_set();
        activity_fds = select(1024, &(this->read_fds), &(this->write_fds), NULL, NULL); 
        if (activity_fds < 0){
            perror("select err: ");
            exit(EXIT_FAILURE);
        }
        if (activity_fds == 0)
            continue ;
        //if there is an activty in this->master_socket then it's a new request
        if (FD_ISSET(this->master_socket, &(this->read_fds))) {
            accept_new_request();
        }
        //loop through this->read_fds if any slot is set then it's a client request
        for (size_t index = 0; index < sockets_FD.size(); index++)
        {
            if(FD_ISSET(sockets_FD[index], &(this->read_fds)) && sockets_FD[index] != this->master_socket ) {
                receive(sockets_FD[index]);
            }
            if(FD_ISSET(sockets_FD[index], &(this->write_fds)) && sockets_FD[index] != this->master_socket ) {
                FD_CLR(sockets_FD[index], &(this->write_fds));
                if(send(sockets_FD[index], response.c_str(), response.length() , 0) < 0) {
                    perror("Send err: ");
                    exit(EXIT_FAILURE);
                }
                close(sockets_FD[index]);
                FD_CLR(sockets_FD[index], &(this->master_fds));
                sockets_FD.erase(sockets_FD.begin() + index);
            }
        }
        usleep(1000);
    }
}
