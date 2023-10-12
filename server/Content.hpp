
#include <string>
#include <sstream>
#include <fstream>
#include <sys/time.h>

#include <iostream>


class Content{
    public:
        Content(std::string &content, std::string &boundary);
        ~Content();
        Content(const Content &copy);
        Content &operator=(const Content &copy);
        void parse_content();

    // private:
        Content();
        std::string content;
        std::string content_type;
        std::string content_name;
        std::string body;
        std::string boundary;
        std::string file_name;
        bool is_file;

};