# pragma once

# include <iostream>
# include <string>
# include <sstream>
# include <vector>
# include <map>
# include <sstream>
# include <fstream>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
# include <unistd.h>
#include <sys/wait.h>

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

        const std::string& getPath() const;
        const std::string&  getRedirectUrl() const;
        const std::vector<std::string>  getMethods() const;
        const bool& getDirectoryListing() const;    
        const std::string&  getDefaultFile() const;

        const std::string& getUploadDirectory() const;
        const bool& getUploadEnabled() const;
        const std::string& get_cgi_bin() const;


        // TODO : add getters
        void    clear();

        void    print();

        bool    empty() {
            if (path.empty() && methods.empty() && directory_listing == false && default_file.empty() && cgi_extensions.empty() && cgi_bin.empty() && upload_enabled == false && upload_directory.empty() && redirect_url.empty())
                return (true);
            return false;
        }
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

class   Servers {
    public:
        Servers();
        ~Servers();
        void    SetHost(const std::string);
        void    SetPort(int);
        void    SetDefault(bool);
        void    SetServerNames(std::vector<std::string>&);
        void    SetErrorPages(std::vector<std::string>&);
        void    SetClientBodyLimit(const std::string&);
        void    SetRoutes(Routes&);
        void    SetSock(int);


        const std::string& getHost() const;
        const int& GetSock() const;
        const int& GetPort() const;
        const bool&    GetDefault() const;
        const std::vector<std::string>& getServerNames() const;
        const std::vector<std::string>& GetErrorPages() const;
        const std::string&  GetClientBodyLimit() const;
        std::vector<Routes>    GetRoutes() const;


        void    print();
        void    clear();
    private:
        std::string                         host;
        int                                 port;
        bool                                is_default;
        std::vector<std::string>            serverNames;
        std::vector<std::string>            error_pages;
        std::string                         clientBodyLimit;
        std::vector<Routes>                 routes;
        int                                 sock;
};

void        split_line(std::map<std::string, std::string>&, std::string , std::string);
void        split_server_names(std::vector<std::string>&, std::string);
void        setup_server_names(std::string&, Servers&);
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
void        setup_routes(std::vector<std::string>, Servers&);
void        CreateServer(std::string, int, Servers&);
std::string TrimSpaces(const std::string&);
