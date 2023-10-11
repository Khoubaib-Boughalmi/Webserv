# include "Servers.hpp"

std::string TrimSpaces(const std::string& input) {
    size_t start = 0;
    size_t end = input.length();

    while (start < end && std::isspace(input[start])) {
        start++;
    }
    while (end > start && (std::isspace(input[end - 1]) || input[end - 1] == ',')) {
        end--;
    }
    return (input.substr(start, end - start));
}

void    split_line(std::map<std::string, std::string>& res, std::string input, std::string del) {
    size_t  pos = 0;
    std::string key;

    input = TrimSpaces(input);
    while ((pos = input.find(del)) != std::string::npos) {
        key = TrimSpaces(input.substr(0, pos));
        input.erase(0, pos + del.length());
        break ;
    }
    std::string value = TrimSpaces(input);
    res[key] = value;
}

void    split_server_names(std::vector<std::string>& res, std::string input) {
    
    std::istringstream iss(input);
    std::string name;
    while (iss >> name) {
        if (name != "|") {
            res.push_back(name);
        }
    }
}

void    setup_server_names(std::string& line, Servers& server) {
    line = TrimSpaces(line);
    std::map<std::string, std::string>  res;
    std::vector<std::string>    s_names;
    std::map<std::string, std::string>::iterator    it;
    split_line(res, line, ":");
    it = res.begin();
    split_server_names(s_names, it->second);
    server.SetServerNames(s_names);
}

bool find_word(const std::string& input, const std::string& targetWord) {
    std::istringstream iss(input);
    std::string word;

    while (iss >> word) {
        if (word == targetWord) {
            return (true);
        }
    }
    return (false);
}
void extract_server(std::string &config, std::string &server)
{
    std::string line;
    std::stringstream ss(config);
    bool first_line;

    first_line = true;
    while (std::getline(ss, line))
    {
        if (line.find("server") == 0 && first_line == false)
            break;
        first_line = false;
        server += line + "\n";
    }
}

void parse_file(std::string config, int servers_nb, std::vector<std::string> &servers)
{
    std::string server;
    for (int i = 0; i < servers_nb; i++)
    {
        extract_server(config, server);
        servers.push_back(server);
        config = config.substr(server.length(), config.length());
        server.clear();
    }
}

void read_file(std::string filename, std::string &conf, int &servers_nb)
{
    std::ifstream ss(filename);
    std::string line;

    if (!ss.is_open())
    {
        std::cerr << "error opening the file" << std::endl;
        exit(1);
    }
    while (std::getline(ss, line))
    {
        if (line.find("server") == 0)
            ++servers_nb;
        conf += line + "\n";
    }
}



void    split_error_pages(std::vector<std::string>& res, std::string input) {
    
    std::istringstream iss(input);
    std::string name;
    while (iss >> name) {
        if (name != "|") {
            res.push_back(TrimSpaces(name));
        }
    }
}

void    setup_error_pages(std::string& line) {
    line = TrimSpaces(line);
    std::map<std::string, std::string>  res;
    std::map<std::string, std::string>::iterator    it;;
    split_line(res, line, ":");
    it = res.begin();
    std::vector<std::string>    error_pages;
    split_error_pages(error_pages, TrimSpaces(it->second));
}

void    split_methods(std::vector<std::string>& res, std::string input) { // repetetive code
    
    std::istringstream iss(input);
    std::string name;
    while (iss >> name) {
        if (name != "|") {
            res.push_back(TrimSpaces(name));
        }
    }
}

void    setup_methods(std::vector<std::string>& methods, std::string& line) {
    line = TrimSpaces(line);
    std::map<std::string, std::string>  res;
    std::map<std::string, std::string>::iterator    it;;
    split_line(res, line, ":");
    it = res.begin();
    split_methods(methods, it->second);

}

void    extract_routes(std::vector<std::string>& rts_vector, std::string server_config) {
    std::stringstream   ss(server_config);
    std::string routes;
    std::string line;
    std::string tmp;

    while (std::getline(ss, line)) {
        if (find_word(line, "routes"))
            break ;
        tmp += line + "\n";
    }
    size_t  f_pos = server_config.find("[");
    size_t  s_pos = server_config.find("]");
    routes = server_config.substr(f_pos, s_pos);

    std::stringstream   rs(routes);
    line.clear();
    tmp.clear();
    while (std::getline(rs, line)) {
        if (find_word(line, "{") || find_word(line, "[") || find_word(line, "}"))
            continue ;
        else if (!find_word(line, "},")) {
            tmp += TrimSpaces(line) + "\n";
        }
        else {
            rts_vector.push_back(tmp);
            tmp.clear();
            continue ;
        }
    }
}

void    fill_routes(Routes& route, std::string routes) {
    std::stringstream   ss(routes);
    std::map<std::string, std::string>  res;
    std::map<std::string, std::string>::iterator it;
    std::string         line;
    std::vector<std::string>    methods;

    while (std::getline(ss, line)) {
        if (find_word(line, "path")) {
            split_line(res, line, ":");
            it = res.begin();
            route.SetPath(TrimSpaces(it->second));
            res.erase(it->first);
        }
        else if (find_word(line, "methods")) {
            setup_methods(methods, line);
            route.SetMethods(methods);
        }
        else if (find_word(line, "directory_listing")) {
            split_line(res, line, ":");
            it = res.begin();
            if (find_word(TrimSpaces(it->second), "true")) {
                route.SetDirectoryListing(true);
            }
            else {
                route.SetDirectoryListing(false);
            }
            res.erase(it->first);
        }
        else if (find_word(line, "default_file")) {
            split_line(res, line, ":");
            it = res.begin();
            route.SetDefaultFile(TrimSpaces(it->second));
            res.erase(it->first);
        }
        else if (find_word(line, "cgi_extensions")) {
            split_line(res, line, ":");
            it = res.begin();
            route.SetCgiExtension(TrimSpaces(TrimSpaces(it->second)));
            res.erase(it->first);
        }
        else if (find_word(line, "cgi_bin")) {
            split_line(res, line, ":");
            it = res.begin();
            route.SetCgiBin(TrimSpaces(it->second));
            res.erase(it->first);
        }
        else if (find_word(line, "upload_enabled")) {
            split_line(res, line, ":");
            it = res.begin();
            if (find_word(it->second, "true")) {
                route.SetUploadEnabled(true);
            }
            else {
                route.SetUploadEnabled(false);
            }
            res.erase(it->first);
        }
        else if (find_word(line, "upload_directory")) {
            split_line(res, line, ":");
            it = res.begin();
            route.SetUploadDirectory(TrimSpaces(it->second));
            res.erase(it->first);
        }
        else if (find_word(line, "redirect_url")) {
            split_line(res, line, ":");
            it = res.begin();
            route.SetRedirectUrl(TrimSpaces(it->second));
            res.erase(it->first);
        }
    }
}

void    setup_routes(std::vector<std::string>   rts_vector, Servers& server) {
    Routes  route;
    std::vector<std::string>::iterator  it = rts_vector.begin();

    while (it != rts_vector.end()) {
        fill_routes(route, *it);
        server.SetRoutes(route);
        route.clear();
        ++it;
    }
}

void    CreateServer(std::string server_config, int sock, Servers& server) {
    std::stringstream   ss(server_config);
    std::string         line;
    std::string         tmp;
    std::map<std::string, std::string>  res;
    std::map<std::string, std::string>::iterator    it;
    std::vector<std::string>    routes;
    std::vector<std::string>::iterator ite;

    extract_routes(routes, server_config);
    while (std::getline(ss, line)) {
        if (find_word(line, "server"))
            continue ;
        else if (find_word(line, "host")) {
            split_line(res, line, ":");
            it = res.begin();
            server.SetHost(it->second);
            res.erase(it->first);
        }
        else if (find_word(line, "port")) {
            split_line(res, line, ":");
            it = res.begin();
            {
                std::stringstream   ss(it->second);
                int tmp;
                if (!(ss >> tmp)) {
                    std::cerr << "Errooooooooòr" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            server.SetPort(std::strtol(it->second.c_str(), NULL, 10));
            res.erase(it->first);
        }
        else if (find_word(line, "default")) {
            split_line(res, line, ":");
            it = res.begin();
            if (find_word(it->second, "true")) {
                server.SetDefault(true);
            }
            else
                server.SetDefault(false);
            res.erase(it->first);
        }
        if (find_word(line, "server_names")) {
            setup_server_names(line, server);
        }
        else if (find_word(line, "error_pages")) {
            setup_error_pages(line);
        }
        else if (find_word(line, "client_body_limit")) {
            split_line(res, line, ":");
            it = res.begin();
            server.SetClientBodyLimit(TrimSpaces(it->second));
            res.erase(it->first);
        }
        else if (find_word(line, "routes")) {
            setup_routes(routes, server);
        }
    }
    server.SetSock(sock);
}
