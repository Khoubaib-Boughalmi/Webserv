# pragma once

# include <iostream>
# include <string>
# include <sstream>
# include <vector>
# include <map>
# include <sstream>
# include <fstream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
# include <unistd.h>
#include <arpa/inet.h>

#define TRUE 1
#define FALSE 0
#define MAX_CONNECTIONS 42

typedef struct sockaddr_in  s_addr_in;
typedef struct sockaddr     s_addr;

class   Routes {
    public:
        Routes();
        ~Routes();

        void    SetPath(const std::string&);
        void    SetMethods(std::vector<std::string>&);
        void    SetDirectoryListing(bool);
        void    SetDefaultFile(const std::string&);
        void    SetCgiExtension(const std::string&);
        void    SetCgiBin(const std::string&);
        void    SetUploadEnabled(bool);
        void    SetUploadDirectory(const std::string&);
        void    SetRedirectUrl(const std::string&);

        // TODO : add getters
        void    clear();

        void    print();
    private:
        // int                         index;
        std::string                 path;
        std::vector<std::string>    methods;
        bool                        directory_listing;
        std::string                 default_file;
        std::string                 cgi_extensions;
        std::string                 cgi_bin;
        bool                        upload_enabled;
        std::string                 upload_directory;
        std::string                 redirect_url;
};

class   ServerData {
    public:
        ServerData();
        ~ServerData();
        void    SetHost(const std::string);
        const std::string& getHost() const;

        void    SetPort(int);
        const int& GetPort() const;

        void    SetDefault(bool);
        const bool&    GetDefault() const;

        void    SetServerNames(std::vector<std::string>&);
        const std::vector<std::string>& getServerNames() const;
        

        void    SetErrorPages(std::vector<std::string>&);
        const std::vector<std::string>& GetErrorPages() const;


        void    SetClientBodyLimit(const std::string&);
        const std::string&  GetClientBodyLimit() const;

        void    SetRoutes(Routes&);
        std::vector<Routes>    GetRoutes() const;


        void    print();

        void    clear();

        // TODO : move this attributes to private section and add setters and getters
        int                 opt;
        socklen_t           addrlen;
        int                 master_socket;
        int                 highest_fd_val;
        std::vector<int>    sockets_FD;

        fd_set              read_fds;
        fd_set              write_fds;
        fd_set              master_fds;
        s_addr_in           address;

        void initialization_and_socket_creation (void);
        void accept_new_request (void);
        void initialize_server_address (void);
        void bind_and_listen (void);
        void init_read_write_fd_set (void);
        void set_non_blocking_socket (int);
        void select_accept_recv_send_handler (void);
        void add_fd_to_master_set (int);
        void receive (int);
        // void send (int, int); // 
        void cleanFDSet (void);

    private:
        std::string                         host;
        int                                 port;
        bool                                is_default;
        std::vector<std::string>            serverNames;
        std::vector<std::string>            error_pages;
        std::string                         clientBodyLimit;
        std::vector<Routes>                 routes;
};

void        split_line(std::map<std::string, std::string>&, std::string , std::string);
void        split_server_names(std::vector<std::string>&, std::string);
void        setup_server_names(std::string&, ServerData&);
bool        find_word(const std::string&, const std::string&);
void        extract_server(std::string &, std::string &);
void        parse_file(std::string , int, std::vector<std::string> &);
void        read_file(std::string, std::string &, int &);
void        split_error_pages(std::vector<std::string>&, std::string);
void        setup_error_pages(std::string&);
void        split_methods(std::vector<std::string>&, std::string);
void        setup_methods(std::vector<std::string>&, std::string&);
void        extract_routes(std::vector<std::string>&, std::string);
void        fill_routes(Routes&, std::string);
void        setup_routes(std::vector<std::string>, ServerData&);
void        fill_class(std::string, std::vector<ServerData>&);
std::string TrimSpaces(const std::string&);