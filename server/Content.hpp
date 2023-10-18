
#include <string>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include "Servers.hpp"

#include <iostream>


class Content{
    public:
        Content(std::string &content, std::string &boundary,Routes &route,size_t limit);
        ~Content();
        Content(const Content &copy);
        Content &operator=(const Content &copy);
        void parse_content(Routes &route,size_t limit);
        int status;

    private:
        Content();
        std::string content;
        std::string content_type;
        std::string content_name;
        std::string body;
        std::string boundary;
        std::string file_name;
        bool is_file;

};