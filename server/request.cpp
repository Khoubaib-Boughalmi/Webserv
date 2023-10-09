#include "request.hpp"

Request::Request() {
    this->req = "";
    this->method = "";
    this->path = "";
    this->protocol = "";
    this->host = "";
    this->connection = "";
    this->cache_control = "";
    this->user_agent = "";
    this->accept = "";
    this->accept_encoding = "";
    this->accept_language = "";
    this->cookie = "";
    this->content_length = "";
    this->content_type = "";
    this->body = "";
}

Request::Request(std::string req) {
    this->req = req;
    bool isBody = false;
    std::stringstream requestStream(req);
    std::string line;

    if (std::getline(requestStream, line)) {
        std::istringstream lineStream(line);
        lineStream >> method >> path >> protocol;
    }

    while (std::getline(requestStream, line)) {
        if (line.empty()) {
            isBody = true;
            continue ;
        }

        if (isBody) {
            this->body.append(line + "\n");
        } else {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string headerName = line.substr(0, pos);
                std::string headerValue = line.substr(pos + 2);

                if (headerName == "Host") {
                    this->host = headerValue;
                } else if (headerName == "Connection") {
                    this->connection = headerValue;
                } else if (headerName == "Cache-Control") {
                    this->cache_control = headerValue;
                } else if (headerName == "User-Agent") {
                    this->user_agent = headerValue;
                } else if (headerName == "Accept") {
                    this->accept = headerValue;
                } else if (headerName == "Accept-Encoding") {
                    this->accept_encoding = headerValue;
                } else if (headerName == "Accept-Language") {
                    this->accept_language = headerValue;
                } else if (headerName == "Cookie") {
                    this->cookie = headerValue;
                } else if (headerName == "Content-Length") {
                    this->content_length = headerValue;
                } else if (headerName == "Content-Type") {
                    this->content_type = headerValue;
                }
            }
        }
    }
}

std::string readHtmlFile(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#include <dirent.h>
# include <unistd.h>

void    HandleDelete(std::string& uri, const int clientFD) {
    // check if uri is directory
    DIR*    dir;
    Response response;
    uri = uri.substr(1, uri.length());
    if (access(uri.c_str(), F_OK) == 0) {
        dir = opendir(uri.c_str());
        // if dir is NULL then the request is file
        if (dir == NULL) {
                if (remove(uri.c_str()) == 0)
                {
                    response.setStatus(204).setBody(readHtmlFile("")).setContentType(getMimeType("html"));
                    response.sendResponse(clientFD);
                    // SetResponse(response, "", "No Content", 204);
                }
                else
                {
                    // SetResponse(response, "", "Internal Server Error", 500);
                }
        }
        else {
            char bck = uri.back();
            if (bck == '/') {
                if (access(uri.c_str(), W_OK) == 0) {
                    if (remove(uri.c_str()) == 0) {
                        // SetResponse(response, "", "No Content", 204);
                    }
                    else{
                        // SetResponse(response, "", "Internal Server Error", 500);
                    }
                }
                else {
                    // SetResponse(response, "", "Forbidden", 403);
                }
            }
            else {
                // SetResponse(response, "", "Conflict", 409);
            }
            closedir(dir);
        }
    }
    else
    {
        response.setStatus(404).setBody(readHtmlFile("static/404.html")).setContentType(getMimeType("html"));
        response.sendResponse(clientFD);
    }
        // SetResponse(response, "404.html", "Not Found", 404);
}

void    Request::parse_request(const int clientFD) {
    if (method == "DELETE")
    {
        HandleDelete(path, clientFD);
    }
    Response    response;
    response.setStatus(200).setBody(readHtmlFile("static/index.html")).setContentType(getMimeType("html"));
    response.sendResponse(clientFD);
}

Request::~Request() {}

Request::Request(const Request &copy) {
    *this = copy;
}

Request &Request::operator=(const Request &copy) {
    if (this != &copy) {
        this->req = copy.req;
        this->method = copy.method;
        this->path = copy.path;
        this->protocol = copy.protocol;
        this->host = copy.host;
        this->connection = copy.connection;
        this->cache_control = copy.cache_control;
        this->user_agent = copy.user_agent;
        this->accept = copy.accept;
        this->accept_encoding = copy.accept_encoding;
        this->accept_language = copy.accept_language;
        this->cookie = copy.cookie;
        this->content_length = copy.content_length;
        this->content_type = copy.content_type;
        this->body = copy.body;
    }
    return *this;
}

std::string Request::get_request(void) {
    return this->req;
}

std::string Request::get_method(void) {
    return this->method;
}

std::string Request::get_path(void) {
    return this->path;
}

std::string Request::get_protocol(void) {
    return this->protocol;
}

std::string Request::get_host(void) {
    return this->host;
}

std::string Request::get_connection(void) {
    return this->connection;
}

std::string Request::get_cache_control(void) {
    return this->cache_control;
}

std::string Request::get_user_agent(void) {
    return this->user_agent;
}

std::string Request::get_accept(void) {
    return this->accept;
}

std::string Request::get_accept_encoding(void) {
    return this->accept_encoding;
}

std::string Request::get_accept_language(void) {
    return this->accept_language;
}

std::string Request::get_cookie(void) {
    return this->cookie;
}

std::string Request::get_content_length(void) {
    return this->content_length;
}

std::string Request::get_content_type(void) {
    return this->content_type;
}

std::string Request::get_body(void) {
    return this->body;
}

void Request::set_method(std::string method) {
    this->method = method;
}

void Request::set_path(std::string path) {
    this->path = path;
}

void Request::set_protocol(std::string protocol) {
    this->protocol = protocol;
}

void Request::set_host(std::string host) {
    this->host = host;
}

void Request::set_connection(std::string connection) {
    this->connection = connection;
}

void Request::set_cache_control(std::string cache_control) {
    this->cache_control = cache_control;
}

void Request::set_user_agent(std::string user_agent) {
    this->user_agent = user_agent;
}

void Request::set_accept(std::string accept) {
    this->accept = accept;
}

void Request::set_accept_encoding(std::string accept_encoding) {
    this->accept_encoding = accept_encoding;
}

void Request::set_accept_language(std::string accept_language) {
    this->accept_language = accept_language;
}

void Request::set_cookie(std::string cookie) {
    this->cookie = cookie;
}

void Request::set_content_length(std::string content_length) {
    this->content_length = content_length;
}

void Request::set_content_type(std::string content_type) {
    this->content_type = content_type;
}

void Request::set_body(std::string body) {
    this->body = body;
}
