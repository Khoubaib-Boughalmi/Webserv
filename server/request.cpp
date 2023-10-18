#include "request.hpp"
#include "client.hpp"
bool containsWord(const std::string& inputString, const std::string& targetWord);
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

Request::Request(std::string req): is_valid(1)
{
    this->req = req;
    std::stringstream requestStream(req);
    std::string line;

    if (std::getline(requestStream, line))
    {
        if (line.empty()) {
            return ;
        }
        std::istringstream lineStream(line);
        if (!(lineStream >> method))
        {
            is_valid = 0;
            return;
        }
        if (!(lineStream >> path))
        {
            is_valid = 0;
            return;
        }

        if (!(lineStream >> protocol))
        {
            is_valid = 0;
            return;
        }
    }
    while (std::getline(requestStream, line))
    {
        size_t pos = line.find(':');
        std::string headerName;
        std::string headerValue;
        if (pos != std::string::npos)
        {
            headerName = line.substr(0, pos);
            if(line.length() > pos + 2)
                headerValue = line.substr(pos + 2);
            else
            {
                is_valid = 0;
                return;
            }

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
            else if (headerName == "Host")
                this->host = TrimSpaces(headerValue);
        }
    }
    fillBodyFromPostRequest(this->req);
}

std::string readFile(const std::string &filename)
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
                response.setStatus(204).setBody(readFile("")).setContentType(getMimeType("html"));
            else
                response.setStatus(500).setBody(readFile("static/error_pages/500.html")).setContentType(getMimeType("html"));
        }
        else
        {
            char bck = uri.back();
            if (bck == '/')
            {
                if (access(uri.c_str(), W_OK) == 0)
                {
                    if (remove(uri.c_str()) == 0)
                        response.setStatus(204).setBody(readFile("")).setContentType(getMimeType("html"));
                    else
                    response.setStatus(500).setBody(readFile("static/error_pages/500.html")).setContentType(getMimeType("html"));
                }
                else
                    response.setStatus(403).setBody(readFile("static/error_pages/403.html")).setContentType(getMimeType("html"));
            }
            else
                response.setStatus(409).setBody(readFile("409.html")).setContentType(getMimeType("html"));
            closedir(dir);
        }
    }
    else
        response.setStatus(404).setBody(readFile("static/404.html")).setContentType(getMimeType("html"));
}

bool is_valid_method(const std::string &method)
{
    if (method == "GET" || method == "POST" || method == "DELETE")
        return (true);
    return (false);
}

bool is_uri_lenght_valid(const std::string &uri)
{
    return (uri.length() < 2048);
}

bool is_uri_size_valid(const std::string &uri)
{
    return ((static_cast<double>(uri.size()) / (1024 * 1024)) < 10);
}

bool is_uri_have_redirect(const std::string &uri)
{
    return (uri == "/redirect");
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

void    HandleRedirect(std::string &uri, Response &response, std::string redirect_uri, Client *clientInfo)
{
    if (clientInfo && matchUriWithPath(uri, clientInfo->server.GetRoutes(), true))
    {
        std::string redirect_url = getRedirect(clientInfo->server.GetRoutes());
        uri = redirect_url;
        if (redirect_url.empty())
            response.setStatus(404).setBody(readFile("static/error_pages/404.html")).setContentType(getMimeType("html"));
        else
            response.setStatus(301).setBody(readFile("static/" + redirect_url)).setContentType(getMimeType("html"));
    }
    else if (!redirect_uri.empty()) {
        uri = redirect_uri;
        response.setStatus(301).setLocation(redirect_uri).setBody(readFile("static/login.html")).setContentType(getMimeType("html"));
    }
    else
        response.setStatus(404).setBody(readFile("static/error_pages/404.html")).setContentType(getMimeType("html"));
}

bool isMethodAllowed(const std::string &method, const std::string &requestedPath, std::vector<Routes> &routes)
{
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        const Routes &route = *it;
        const std::string &path = route.getPath();
        if (path == requestedPath)
        {
            std::vector<std::string> methods = route.getMethods();
            std::vector<std::string>::iterator it ;
            for (it = methods.begin(); it != methods.end(); it++) {
                if(*it == method)
                    return true;
            }            
        }
    }
    return false;
}

bool is_directory(std::string &uri)
{
    // uri = uri.substr(7, uri.length());
    DIR *dir;

    dir = opendir(uri.c_str());
    if (dir == NULL)
        return false;
    closedir(dir);
    return (true);
}

bool containsWord(const std::string& inputString, const std::string& targetWord) {
    std::size_t found = inputString.find(targetWord);
    return found != std::string::npos;
}

bool has_cgi(const std::string &uri)
{
    if (containsWord(uri, "cgi-bin"))
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
        const std::string &path = route.getPath();
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

std::string GetFileExtension(const std::string& filename) {
    std::size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos + 1);
    }
    return "html";
}

std::string GetDefaultFile(const std::vector<Routes>& routes, const std::string& uri) {
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
        if (it->getPath() == uri) {
            return it->getDefaultFile();
        }
    }
    return "";
}
/***/
void    cgi_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl err: "); // to be changed to log
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) { // modify it to be compatible with the subject
        perror("fcntl err: ");
        exit(EXIT_FAILURE);
    }
}

void    send_cgibin_response(std::string& body, const char *abs_path, std::string& rooted_uri, Response& cresponse) {

    std::string username;
    std::string password;
    if (!body.empty())
    {
        username = body.substr(9, body.find("&") - 9);
        password = body.substr(body.find("&") + 10, body.length() - 1);
    }
    int status;
    int child_fd;
    char buff[10240] = {0};
    int pipefd[2];

    std::string response;
    std::string directory_path = "static";
    // rooted_uri = "/static/cgi-bin/index.php";

    if (!containsWord(rooted_uri, "static")) {
        rooted_uri = directory_path += rooted_uri;
    }
        // rooted_uri = "/static/cgi-bin/index.php";
    if(pipe(pipefd) < 0) {
        perror("pipe err: ");
        exit(EXIT_FAILURE);
    }
    cgi_non_blocking(pipefd[0]);
    cgi_non_blocking(pipefd[1]);
    std::string path;
    child_fd = fork();
    if(!child_fd) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        path =  rooted_uri;
        char *args[] = {
            (char *)abs_path,
            (char *)path.c_str(),
            (char *)username.c_str(),
            (char *)password.c_str(),
            // (char *)"hello",
            // (char *)"world",
            NULL
        };
        if (execve(abs_path, args, NULL) < 0) {
            perror("execv err: ");
            exit(EXIT_FAILURE);
        }
    }
    else {
        close(pipefd[1]);
        waitpid(child_fd, &status, 0);
        while (read(pipefd[0], buff, sizeof(buff)) != 0) {
            response += buff;
            memset(buff, 0, sizeof(buff));
        }
        close(pipefd[0]);
        std::ostringstream oss;
        oss << response.length();
        std::string content_length = oss.str();
        cresponse.setStatus(status).setLocation("/login")
                .setBody(response)
                .setContentType(getMimeType(GetFileExtension("html")));
    }
}

/***/
std::string    get_abs_path(const std::string &requestedPath, std::vector<Routes> &routes)
{
    Routes route;
    std::string abs_path;
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        route = *it;
        const std::string &path = route.getPath();
        if (path == requestedPath)
        {
            return route.get_cgi_bin();
        }
    }
    return abs_path;
}

void HandleGet(std::string uri, std::vector<Routes> &routes, Request& request, Response &response)
{
    std::string rooted_uri = "static" + uri;
    if (matchUriWithPath(uri, routes, false) == false) {
        response.setStatus(404).setLocation(uri)
            .setBody(readFile("static/error_pages/404.html"))
            .setContentType(getMimeType("html"));
    }
    else if (has_cgi(rooted_uri)) {
        std::string abs_path = get_abs_path(uri, routes);
        if (abs_path.empty())
            response.setStatus(500).setLocation(uri)
                .setBody(readFile("static/error_pages/500.html"))
                .setContentType(getMimeType("html"));
        else
        {
            {
                std::string tmp;
                send_cgibin_response(tmp, abs_path.c_str(), rooted_uri, response);
            }
        }
    }
    else if (is_directory(rooted_uri) && uri != "/")
    {
        if (directoryHasIndexFile(rooted_uri)) {
            response.setStatus(200).setLocation(uri)
                .setBody(readFile(rooted_uri.append("/") + GetDefaultFile(routes, uri)))
                .setContentType(getMimeType(GetFileExtension(uri)));
        }
        else
        {
            if (directory_listing(uri, routes)) {
                response.setStatus(200).setLocation(uri)
                    .setBody(generateDirectoryListing("static" + uri))
                    .setContentType(getMimeType("html"));
                return ;
            }
            else
            {
                response.setStatus(403)
                    .setBody(readFile("static/error_pages/404.html"))
                    .setContentType(getMimeType("html"));
            }
        }
    }
    else
    {
        if (uri == "/login")
        {
            if (check_cookies(request.get_cookie())) {
                HandleRedirect(uri, response, "/", NULL);
            }
            else
                response.setStatus(200).setLocation(uri)
                    .setBody(readFile("static/" + GetDefaultFile(routes, uri)))
                    .setContentType(getMimeType(GetFileExtension(uri)));
                // response.setStatus(200).setLocation("/login").setBody(readFile("static/login.html")).setContentType(getMimeType("html"));
        }
        else
        {
            if (check_cookies(request.get_cookie())) {
                if (uri == "/") {
                    response.setStatus(200).setLocation(uri).setLocation("/")
                        .setBody(readFile("static/" + GetDefaultFile(routes, uri)))
                        .setContentType(getMimeType(GetFileExtension(uri)));
                }
                else {
                    response.setStatus(200).setLocation(uri).setLocation(uri)
                        .setBody(readFile("static/" + GetDefaultFile(routes, uri)))
                        .setContentType(getMimeType(GetFileExtension(uri)));
                    // response.setStatus(200).setLocation(uri).setBody(readFile(GetDefaultFile(routes, uri))).setContentType(getMimeType(GetFileExtension(uri)));
                }
            }
            else {
                HandleRedirect(uri, response, "/login", NULL);
            }
        }
    }
}

void find_all_centent(std::string req,std::string boundary,Routes &route)
{
    std::vector<Content> contents;
    size_t start_pos = req.find(boundary);
    size_t next_pos ;
    while(start_pos != std::string::npos)
    {
        next_pos = req.find(boundary, start_pos + 1);
        if(start_pos != std::string::npos &&next_pos != std::string::npos)
        {
            std::string content = req.substr(start_pos, next_pos - start_pos);
            Content tmp(content, boundary,route);
            contents.push_back(tmp);
            
        }
        start_pos = next_pos;
    }
}

void    Post_handle_form_data(std::string req, Response response,Routes &route) {
    if (req.find("Content-Type: multipart/form-data") != std::string::npos) {
            std::string boundary_start = "boundary=";
            size_t boundary_pos = req.find(boundary_start);
            if (boundary_pos != std::string::npos) {
                boundary_pos += boundary_start.length();
                std::string boundary = "--" + req.substr(boundary_pos, req.find("\r\n", boundary_pos) - boundary_pos);
                find_all_centent(req, boundary,route);
            }
        }
    response.setStatus(200).setBody(readFile("static/index.html")).setContentType(getMimeType("html"));

}

Routes Route_returning(const std::string &requestedPath, std::vector<Routes> &routes)
{
    Routes route;
    for (std::vector<Routes>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        route = *it;
        const std::string &path = route.getPath();
        if (path == requestedPath)
        {
            return route;
        }
    }
    return route;
}

int HandlePost(Request &request, Response &response, Client *clientInfo,std::string uri, std::vector<Routes> &routes)
{
    (void)clientInfo;
    (void)response;
    Response    res;
    Routes route = Route_returning(uri, routes);

    if(request.get_request().find("Content-Type: multipart/form-data") != std::string::npos)
    {
        Post_handle_form_data(request.get_request(), response,route);
    }
    if (has_cgi(uri)) {
        std::string abs_path = get_abs_path(uri, routes);
        {
            std::string body = request.get_body();
            send_cgibin_response(body, abs_path.c_str(), uri, response);
        }
    }
    else
    {
        if (request.get_body().empty())
            return (0);
        std::string username;
        std::string password;
        if (route.getPath() == "/login") {
            username = request.get_body().substr(9, request.get_body().find("&") - 9);
            password = request.get_body().substr(request.get_body().find("&") + 10, request.get_body().length() - 1);
        }
        if (username == "khoubaib" && password == "123456789")
        {
            std::string cookie;
            if (containsWord(request.get_host(), "localhost")) {
                cookie = "sessionToken=abc123; Domain=localhost; Path=/; Expires=Wed, 09 Jun 2024 10:18:14 GMT";
            }
            else {
                cookie = "sessionToken=abc123; Domain=10.12.10.25; Path=/; Expires=Wed, 09 Jun 2024 10:18:14 GMT";
            }
            response.setStatus(301).setLocation("/").setCookie(cookie).setBody(readFile("static/index.html")).setContentType(getMimeType("html"));
            return (1);
        }
        else
            response.setStatus(401).setBody(readFile("static/login.html")).setContentType(getMimeType("html"));
    }
    
    return (0);
}

void parse_request(Request &request, Client *clientInfo) // TODO: check request for inprintable characters
{
    Response response;
    std::string uri;
    std::vector<Routes> routes = clientInfo->server.GetRoutes();
    // if (request.get_path().length() >= 1)
    uri = request.get_path();
    // else
    // {
    //     response.setStatus(500).setBody(readFile("static/error_pages/500.html")).setContentType(getMimeType("html"));
    //     response.sendResponse(clientInfo->clientFD);
    //     return ;
    // }
    if (!is_valid_method(request.get_method()))
        response.setStatus(501)
            .setBody(readFile("static/error_pages/501.html"))
            .setContentType(getMimeType("html"));
    else if (!is_uri_lenght_valid(uri))
        response.setStatus(414)
            .setBody(readFile("static/414.html"))
            .setContentType(getMimeType("html"));
    else if (!is_uri_size_valid(uri))
        response.setStatus(413)
            .setBody(readFile("static/413.html"))
            .setContentType(getMimeType("html"));
    else if (is_uri_have_redirect(uri)) {
        HandleRedirect(uri, response, "/", clientInfo);
    }
    else if (!isMethodAllowed(request.get_method(), uri, routes))
        response.setStatus(405).setBody(readFile("static/error_pages/405.html")).setContentType(getMimeType("html"));
    else if (request.get_method() == "GET")
    {
        HandleGet(uri, routes, request, response);
    }
    else if (request.get_method() == "POST")
    {
        HandlePost(request, response, clientInfo, uri, routes);
    }
    else if (request.get_method() == "DELETE")
    {
        HandleDelete(uri, response);
    }
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
        this->is_valid = copy.is_valid;
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

int Request::get_is_valid()
{
    return this->is_valid;
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
