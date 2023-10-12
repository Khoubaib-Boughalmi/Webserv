#include "request.hpp"
#include "client.hpp"

Request::Request() {}

void Request::fillBodyFromPostRequest(const std::string &request)
{
    // Find the position of the double newline separator
    size_t bodyStartPos = request.find("\r\n\r\n");
    if (bodyStartPos != std::string::npos)
    {
        // Extract the body from the request string
        std::string requestBody = request.substr(bodyStartPos + 4);
        // Set the body member of the Request object
        this->body = requestBody;
    }
}

Request::Request(std::string req)
{
    this->req = req;
    std::stringstream requestStream(req);
    std::string line;

    if (std::getline(requestStream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> method >> path >> protocol;
    }

    while (std::getline(requestStream, line))
    {
        size_t pos = line.find(':');
        if (pos != std::string::npos)
        {
            std::string headerName = line.substr(0, pos);
            std::string headerValue = line.substr(pos + 2);

            if (headerName == "Servers")
                this->host = TrimSpaces(headerValue);
            else if (headerName == "Connection")
                this->connection = TrimSpaces(headerValue);
            else if (headerName == "Cache-Control")
                this->cache_control = TrimSpaces(headerValue);
            else if (headerName == "User-Agent")
                this->user_agent = TrimSpaces(headerValue);
            else if (headerName == "Accept")
                this->accept = TrimSpaces(headerValue);
            else if (headerName == "Accept-Encoding")
                this->accept_encoding = TrimSpaces(headerValue);
            else if (headerName == "Accept-Language")
                this->accept_language = TrimSpaces(headerValue);
            else if (headerName == "Cookie")
                this->cookie = TrimSpaces(headerValue);
            else if (headerName == "Content-Length")
                this->content_length = TrimSpaces(headerValue);
            else if (headerName == "Content-Type")
                this->content_type = TrimSpaces(headerValue);
        }
        fillBodyFromPostRequest(this->req);
    }
}

std::string readHtmlFile(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void HandleDelete(std::string &uri, Response &response)
{
    DIR *dir;
    if (access(uri.c_str(), F_OK) == 0)
    {
        // check if uri is directory
        dir = opendir(uri.c_str());
        // if dir is NULL then the request is file
        if (dir == NULL)
        {
            if (remove(uri.c_str()) == 0)
                response.setStatus(204).setBody(readHtmlFile("")).setContentType(getMimeType("html"));
            else
                response.setStatus(500).setBody(readHtmlFile("500.html")).setContentType(getMimeType("html"));
        }
        else
        {
            char bck = uri.back();
            if (bck == '/')
            {
                if (access(uri.c_str(), W_OK) == 0)
                {
                    if (remove(uri.c_str()) == 0)
                        response.setStatus(204).setBody(readHtmlFile("")).setContentType(getMimeType("html"));
                    else
                        response.setStatus(500).setBody(readHtmlFile("")).setContentType(getMimeType("html"));
                }
                else
                    response.setStatus(403).setBody(readHtmlFile("403.html")).setContentType(getMimeType("html"));
            }
            else
                response.setStatus(409).setBody(readHtmlFile("409.html")).setContentType(getMimeType("html"));
            closedir(dir);
        }
    }
    else
        response.setStatus(404).setBody(readHtmlFile("static/404.html")).setContentType(getMimeType("html"));
}

bool is_valid_method(const std::string &method)
{
    return (method == "GET" || method == "POST" || method == "DELETE");
}

bool is_uri_lenght_valid(const std::string &uri)
{
    return (uri.length() < 2048);
}

bool is_uri_size_valid(const std::string &uri)
{
    return ((static_cast<double>(uri.size()) / (1024 * 1024)) < 10);
}

bool is_valid_location(const std::string &uri)
{
    // if (uri != "login" && uri != "favicon.ico" && !uri.empty())
    //     return (false);
    // return (true);
    if (access(uri.c_str(), F_OK) == 0 || uri.empty())
        return (true);
    return (false);
}

bool is_uri_have_redirect(const std::string &uri)
{
    return (uri == "redirect");
}

bool matchUriWithPath(const std::string &uri, const std::vector<Routes> &routes, bool redirect)
{
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        const Routes &route = *it;
        const std::string &path = route.getPath();
        if (redirect)
        {
            if (path == "/redirect")
                return true;
        }
        else
        {
            if (uri == path)
                return true;
        }
    }
    return false;
}

std::string getRedirect(const std::vector<Routes> &routes)
{
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        const Routes &route = *it;
        const std::string &path = route.getPath();
        if (path == "/redirect")
        {
            return route.getRedirectUrl();
        }
    }
    return "";
}

void HandleRedirect(std::string &uri, Response &response, std::string redirect_uri, Client *clientInfo)
{
    if (clientInfo && matchUriWithPath(uri, clientInfo->server.GetRoutes(), true))
    {
        std::string redirect_url = getRedirect(clientInfo->server.GetRoutes());
        uri = redirect_url;
        if (redirect_url.empty())
            response.setStatus(404).setBody(readHtmlFile("static/error_pages/404.html")).setContentType(getMimeType("html"));
        else
            response.setStatus(301).setBody(readHtmlFile("static/error_pages/301.html")).setContentType(getMimeType("html"));
    }
    else if (!redirect_uri.empty()) {
        uri = redirect_uri;
        response.setStatus(301).setLocation(redirect_uri).setBody(readHtmlFile("static/error_pages/301.html")).setContentType(getMimeType("html"));
    }
    else
        response.setStatus(404).setBody(readHtmlFile("static/error_pages/404.html")).setContentType(getMimeType("html"));
}

bool isMethodAllowed(const std::string &method, const std::string &requestedPath, std::vector<Routes> &routes)
{
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        const Routes &route = *it;
        const std::string &path = route.getPath().substr(1);
        if (path == requestedPath)
        {
            std::vector<std::string> methods = route.getMethods();
            std::vector<std::string>::iterator it = std::find(methods.begin(), methods.end(), method);
            if (it != methods.end())
                return true;
        }
    }
    return false;
}

bool is_directory(const std::string &uri)
{
    DIR *dir;

    dir = opendir(uri.c_str());
    if (dir == NULL)
        return false;
    closedir(dir);
    return (true);
}

bool has_cgi(const std::string &uri)
{
    if (uri == "cgi-bin")
        return (true);
    return false;
}

bool directoryHasIndexFile(const std::string &directoryPath)
{
    DIR *dir = opendir(directoryPath.c_str());

    if (dir)
    {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_REG && std::strcmp(entry->d_name, "index.html") == 0)
            {
                closedir(dir);
                return true;
            }
        }
        closedir(dir);
    }
    return false;
}

bool directory_listing(const std::string &requestedPath, std::vector<Routes> &routes)
{
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        const Routes &route = *it;
        const std::string &path = route.getPath().substr(1);
        if (path == requestedPath)
        {
            if (route.getDirectoryListing())
                return true;
            return false;
        }
    }
    return false;
}

int check_cookies(std::string cookie)
{
    if (cookie.empty())
        return (0);
    std::string cookieValue = cookie.substr(cookie.find("=") + 1, cookie.length());
    if (cookieValue == "abc123")
        return (1);
    return (0);
}

std::string generateDirectoryListing(const std::string &directory)
{
    std::stringstream html;
    html << "<html>"
            "<head>"
            "<title>Directory Listing</title>"
            "</head>"
            "<body style=\"font-family: Arial, sans-serif; background-color: #f3f3f3; padding: 20px; margin: 0;\">"
            "<h1 style=\"font-size: 24px; margin-bottom: 20px;\">Directory Listing</h1>"
            "<ul style=\"list-style-type: none; padding: 0;\">";

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory.c_str())) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            std::string filename(entry->d_name);

            // Skip the current and parent directory entries
            if (filename != "." && filename != "..")
            {
                html << "<li style=\"margin-bottom: 10px; background-color: #fff; border-radius: 5px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); padding: 10px; display: flex; align-items: center;\">"
                     << filename
                     << "</li>";
            }
        }

        closedir(dir);
    }
    else
    {
        html << "Error: Unable to open directory";
    }

    html << "</ul>"
            "</body>"
            "</html>";
    return html.str();
}

void HandleGet(std::string uri, std::vector<Routes> &routes, Request& request, Response &response)
{
    // if (!is_valid_location(uri))
    // {
    //     std::cout << "Not found\n";
    //     response.setStatus(404).setLocation(uri)
    //         .setBody(readHtmlFile("static/error_pages/404.html"))
    //         .setContentType(getMimeType("html"));
    // }
    std::cout << "URIIIIIIII: " << uri << std::endl;
    if (matchUriWithPath("/" + uri, routes, false) == false) {
        std::cout << "Not found\n";
        response.setStatus(404).setLocation(uri)
            .setBody(readHtmlFile("static/error_pages/404.html"))
            .setContentType(getMimeType("html"));
    }
    else if (is_directory(uri))
    {
        std::cout << "DIR" << std::endl;
        if (directoryHasIndexFile(uri))
            std::cout << "Has index file\n";
        else
        {
            std::cout << "Has Not index file\n";
            if (directory_listing(uri, routes)) {
                std::cout << "Directory Listing true\n";
                response.setStatus(200).setLocation(uri)
                    .setBody(generateDirectoryListing(uri))
                    .setContentType(getMimeType("html"));
                return ;
            }
            else
            {
                response.setStatus(403)
                    .setBody(readHtmlFile("static/404.html"))
                    .setContentType(getMimeType("html"));
            }
        }
    }
    else
    {
        std::cout << "URI " << uri << std::endl; 
        if (uri == "login")
        {
            if (check_cookies(request.get_cookie()))
                HandleRedirect(uri, response, "/", NULL);
            else
                response.setStatus(200).setLocation("/login").setBody(readHtmlFile("static/login.html")).setContentType(getMimeType("html"));
        }
        else
        {
            if (check_cookies(request.get_cookie())) {
                if (uri.empty())
                    response.setStatus(200).setLocation(uri).setBody(readHtmlFile("static/index.html")).setContentType(getMimeType("html"));
                else
                    response.setStatus(200).setLocation(uri).setBody(readHtmlFile("static/blog.html")).setContentType(getMimeType("html"));
            }
            else {
                HandleRedirect(uri, response, "/login", NULL);
            }
        }
    }
    // else
    // {
    //     if (has_cgi(uri))
    //         std::cout << "CGI" << std::endl;
    //     else
    //         response.setStatus(200)
    //             .setBody(readHtmlFile("static/404.html"))
    //             .setContentType(getMimeType("html"));
    // }
}

int HandlePost(Request &request, Response *response, Client *clientInfo)
{
    (void)clientInfo;
    (void)response;
    if (request.get_body().empty())
        return (0);
    std::string username = request.get_body().substr(9, request.get_body().find("&") - 9);
    std::string password = request.get_body().substr(request.get_body().find("&") + 10, request.get_body().length() - 1);
    std::cout << username << std::endl;
    std::cout << password << std::endl;
    if (username == "khoubaib" && password == "123456789")
    {
        std::string cookie;
        cookie = "sessionToken=abc123; Domain=localhost; Path=/; Secure; HttpOnly;  Expires=Wed, 09 Jun 2024 10:18:14 GMT";
        response->setStatus(301).setLocation("/").setCookie(cookie).setBody(readHtmlFile("static/index.html")).setContentType(getMimeType("html"));
        return (1);
    }
    else
        response->setStatus(401).setBody(readHtmlFile("static/login.html")).setContentType(getMimeType("html"));
    return (0);
}

void parse_request(Request &request, Client *clientInfo) // TODO: check request for inprintable characters
{
    Response response;
    std::vector<Routes> routes = clientInfo->server.GetRoutes();
    std::cout << "BEFORE: " << request.get_path() << std::endl;
    std::string uri = request.get_path().substr(1);

    // if (!is_valid_method(request.get_method()))
    //     response.setStatus(501)
    //         .setBody(readHtmlFile("static/501.html"))
    //         .setContentType(getMimeType("html"));
    // else if (!is_uri_lenght_valid(uri))
    //     response.setStatus(414)
    //         .setBody(readHtmlFile("static/414.html"))
    //         .setContentType(getMimeType("html"));
    // else if (!is_uri_size_valid(uri))
    //     response.setStatus(413)
    //         .setBody(readHtmlFile("static/413.html"))
    //         .setContentType(getMimeType("html"));
    // else if (!is_valid_location(uri))
    //     response.setStatus(404)
    //         .setBody(readHtmlFile("static/404.html"))
    //         .setContentType(getMimeType("html"));
    // else if (is_uri_have_redirect(uri))
    //     HandleRedirect(uri, response, clientInfo);
    // else if (!isMethodAllowed(request.get_method(), uri, routes))
    //     response.setStatus(405).setBody(readHtmlFile("static/404.html")).setContentType(getMimeType("html"));
    if (request.get_method() == "GET")
    {
        HandleGet(uri, routes, request, response);
    }
    else if (request.get_method() == "POST")
    {
        HandlePost(request, &response, clientInfo);
    }
    // else if (request.get_method() == "DELETE")
    //     HandleDelete(uri, response);
    // std::cout << "IMMMMMM\n";
    // if(!alreadyResponded)
    //     response.setStatus(200).setBody(readHtmlFile("static/index.html")).setContentType(getMimeType("html"));
    response.sendResponse(clientInfo->clientFD);
}

Request::~Request() {}

Request::Request(const Request &copy)
{
    *this = copy;
}

Request &Request::operator=(const Request &copy)
{
    if (this != &copy)
    {
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

std::string Request::get_request(void)
{
    return this->req;
}

std::string Request::get_method(void)
{
    return this->method;
}

std::string Request::get_path(void)
{
    return this->path;
}

std::string Request::get_protocol(void)
{
    return this->protocol;
}

std::string Request::get_host(void)
{
    return this->host;
}

std::string Request::get_connection(void)
{
    return this->connection;
}

std::string Request::get_cache_control(void)
{
    return this->cache_control;
}

std::string Request::get_user_agent(void)
{
    return this->user_agent;
}

std::string Request::get_accept(void)
{
    return this->accept;
}

std::string Request::get_accept_encoding(void)
{
    return this->accept_encoding;
}

std::string Request::get_accept_language(void)
{
    return this->accept_language;
}

std::string Request::get_cookie(void)
{
    return this->cookie;
}

std::string Request::get_content_length(void)
{
    return this->content_length;
}

std::string Request::get_content_type(void)
{
    return this->content_type;
}

std::string Request::get_body(void)
{
    return this->body;
}

void Request::set_method(std::string method)
{
    this->method = method;
}

void Request::set_path(std::string path)
{
    this->path = path;
}

void Request::set_protocol(std::string protocol)
{
    this->protocol = protocol;
}

void Request::set_host(std::string host)
{
    this->host = host;
}

void Request::set_connection(std::string connection)
{
    this->connection = connection;
}

void Request::set_cache_control(std::string cache_control)
{
    this->cache_control = cache_control;
}

void Request::set_user_agent(std::string user_agent)
{
    this->user_agent = user_agent;
}

void Request::set_accept(std::string accept)
{
    this->accept = accept;
}

void Request::set_accept_encoding(std::string accept_encoding)
{
    this->accept_encoding = accept_encoding;
}

void Request::set_accept_language(std::string accept_language)
{
    this->accept_language = accept_language;
}

void Request::set_cookie(std::string cookie)
{
    this->cookie = cookie;
}

void Request::set_content_length(std::string content_length)
{
    this->content_length = content_length;
}

void Request::set_content_type(std::string content_type)
{
    this->content_type = content_type;
}

void Request::set_body(std::string body)
{
    this->body = body;
}
