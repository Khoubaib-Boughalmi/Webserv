#include "Content.hpp"

Content::Content(std::string &content, std::string &boundary, Routes &route) {
    this->content = content;
    this->boundary = boundary;
    this->is_file = false;
    this->parse_content(route);
}

Content::Content() {}
Content::~Content() {}
Content::Content(const Content &copy) {
    *this = copy;
}
Content &Content::operator=(const Content &copy) {
    if (this != &copy) {
        this->content = copy.content;
        this->content_type = copy.content_type;
        this->content_name = copy.content_name;
        this->body = copy.body;
        this->boundary = copy.boundary;
        this->file_name = copy.file_name;
        this->is_file = copy.is_file;
    }
    return *this;
}

void Content::parse_content(Routes &route) {
    int lineinfopos=this->content.find("Content-Disposition:");
    std::string lineInfo=this->content.substr(lineinfopos,this->content.find("\r\n",lineinfopos)-lineinfopos);

    size_t pos = this->content.find("\r\n\r\n");
    if(pos != std::string::npos) {
        this->body = this->content.substr(pos + 4,this->content.length() - pos - 6);
    }
    std::string ForName="\r\n";
    pos = lineInfo.find("filename=");
    if(pos != std::string::npos) {
        this->file_name = lineInfo.substr(pos + 10, lineInfo.find("\"", pos+10) - pos - 10);
        this->is_file = true;
        ForName = ";";
    }
    pos = lineInfo.find("name=");

    if(pos != std::string::npos) {
        this->content_name = lineInfo.substr(pos + 6, lineInfo.find("\"", pos + 6) - pos - 6);
    }
    // check folder exist
    // struct stat st = {0};
    // if (stat("./static/upload", &st) == -1) {
    //     mkdir("./static/upload", 0700);
    // }
    if(this->is_file && route.getUploadEnabled())
    {
        std::ofstream myfile;
        myfile.open("static/" + route.getUploadDirectory() + this->file_name);
        myfile << this->body;
        myfile.close();
    }
}