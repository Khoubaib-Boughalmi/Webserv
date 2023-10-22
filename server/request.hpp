#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstring>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include "Response.hpp"
#include <stdexcept> 
#include "Servers.hpp"
#include "Content.hpp"


class Request
{
    private:
        int is_valid;
        std::string req;
        std::string method;
        std::string path;
        std::string protocol;
        std::string host;
        std::string connection;
        std::string cache_control;
        std::string user_agent;
        std::string accept;
        std::string accept_encoding;
        std::string accept_language;
        std::string cookie;
        std::string content_length;
        std::string content_type;
        std::string body;
    public:
        Request();
        Request(std::string req);
        Request(const Request &copy);
        Request &operator=(const Request &copy);
        ~Request();
        std::string get_request(void);
        std::string get_method(void);
        std::string get_path(void);
        std::string get_protocol(void);
        std::string get_host(void);
        std::string get_connection(void);
        std::string get_cache_control(void);
        std::string get_user_agent(void);
        std::string get_accept(void);
        std::string get_accept_encoding(void);
        std::string get_accept_language(void);
        std::string get_cookie(void);
        std::string get_content_length(void);
        std::string get_content_type(void);
        std::string get_body(void);
        int         get_is_valid(void);

        void set_method(std::string method);
        void set_path(std::string path);
        void set_protocol(std::string protocol);
        void set_host(std::string host);
        void set_connection(std::string connection);
        void set_cache_control(std::string cache_control);
        void set_user_agent(std::string user_agent);
        void set_accept(std::string accept);
        void set_accept_encoding(std::string accept_encoding);
        void set_accept_language(std::string accept_language);
        void set_cookie(std::string cookie);
        void set_content_length(std::string content_length);
        void set_content_type(std::string content_type);
        void set_body(std::string body);
        void fillBodyFromPostRequest(const std::string& request);
        void print_request(void);
};
std::string GetFileExtension(const std::string& filename);
#endif
