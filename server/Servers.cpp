# include "Servers.hpp"

Servers::Servers() {}

void   Servers::SetHost(const std::string server_host) { host = server_host; }
const std::string& Servers::getHost() const { return host; }

void    Servers::SetPort(int server_port) { port = server_port; }
const int& Servers::GetPort() const { return port; }

void    Servers::SetDefault(bool default_server) { is_default = default_server; }
const bool&    Servers::GetDefault() const { return is_default; }

void    Servers::SetServerNames(std::vector<std::string>& s_names) {
    std::vector<std::string>::iterator  it = s_names.begin();
    while (it != s_names.end()) {
        serverNames.push_back(*it);
        ++it;
    }
}

const std::vector<std::string>& Servers::getServerNames() const { return serverNames; }

void    Servers::SetErrorPages(std::vector<std::string>& err_pages) {
     std::vector<std::string>::iterator  it = err_pages.begin();
    while (it != err_pages.end()) {
        error_pages.push_back(*it);
        ++it;
    }
}
const std::vector<std::string>& Servers::GetErrorPages() const { return error_pages; }

void    Servers::SetClientBodyLimit(const std::string& bodyLimit) { clientBodyLimit = bodyLimit; }
const std::string&  Servers::GetClientBodyLimit() const { return clientBodyLimit; }


void    Servers::SetRoutes(Routes& route) {
    routes.push_back(route);
}
std::vector<Routes>    Servers::GetRoutes() const { return routes; }

void    Servers::print() {
    std::cout << "\x1b[1;33m╔═════════════════════════════════════════╗\x1b[0m\n";
    std::cout << "\x1b[1;33m║          Servers Configuration       ║\x1b[0m\n";
    std::cout << "\x1b[1;33m╠═════════════════════════════════════════╣\x1b[0m\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mHost               : \x1b[0m" << host << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mPort               : \x1b[0m" << port << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mDefault Server     : \x1b[0m" << (is_default ? "\x1b[1;32mtrue\x1b[0m" : "\x1b[1;31mfalse\x1b[0m") << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mClient Body Limit  : \x1b[0m" << clientBodyLimit << "\n";
    
    int x = 1;
    {
        std::vector<Routes>::const_iterator rit = routes.begin();
        while (rit != routes.end()) {
            std::cout << "\x1b[1;33m║ ----Route " << x++ << "-------\x1b[0m\n";
            Routes tmp = *rit;
            tmp.print();
            ++rit;
        }
    }
    
    std::cout << "\x1b[1;33m╚═════════════════════════════════════════╝\x1b[0m\n";
}

void    Servers::clear() {
    host.clear();
    port = 0;
    is_default = false;
    serverNames.clear();
    error_pages.clear();
    clientBodyLimit.clear();
    routes.clear();
}

void    Servers::SetSock(int s) {
    sock = s;
}

const int& Servers::GetSock() const {
    return sock;
}

const std::string&  Routes::getRedirectUrl() const {
    return (this->redirect_url);
}
const std::vector<std::string>  Routes::getMethods() const {
    return (this->methods);
}

Servers::~Servers() {}

//
Routes::Routes() {}

void    Routes::SetPath(const std::string& pth) { path = pth; }

void    Routes::SetMethods(std::vector<std::string>& m) { 
    std::vector<std::string>::iterator  it = m.begin();
    while (it != m.end()) {
        methods.push_back(*it);
        ++it;
    }
}

void    Routes::SetDirectoryListing(bool d_lst) { directory_listing = d_lst; }

void    Routes::SetDefaultFile(const std::string& d_file) { default_file = d_file; } 

void    Routes::SetCgiExtension(const std::string& cgi_ext) { cgi_extensions = cgi_ext; }

void    Routes::SetCgiBin(const std::string& cgi_b) { cgi_bin = cgi_b; }

void    Routes::SetUploadEnabled(bool up_en) { upload_enabled = up_en; }

void    Routes::SetUploadDirectory(const std::string& up_dir) { upload_directory = up_dir; }

void    Routes::SetRedirectUrl(const std::string& r_url) { redirect_url = r_url; }

const std::string& Routes::getPath() const {
    return path;
}

void    Routes::clear() {
    path.clear();
    methods.clear();
    directory_listing = false;
    default_file.clear();
    cgi_extensions.clear();
    cgi_bin.clear();
    upload_enabled = false;
    upload_directory.clear();
    redirect_url.clear();
}

void    Routes::print() {
    std::cout << "\x1b[1;33m║ \x1b[1;34mPath        : \x1b[0m" << path << "\n";

    std::cout << "\x1b[1;33m║ \x1b[1;34mMethods     : \x1b[0m";
    std::vector<std::string>::iterator it;
    it = methods.begin();
    while (it != methods.end()) {
        std::cout << *it;
        ++it;
        if (it != methods.end()) {
            std::cout << "\t";
        }
    }
    std::cout << "\n";
    
    std::cout << "\x1b[1;33m║ \x1b[1;34mDirectory Listing : \x1b[0m" << (directory_listing ? "\x1b[1;32mtrue\x1b[0m" : "\x1b[1;31mfalse\x1b[0m") << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mDefault File : \x1b[0m" << default_file << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mCGI Extensions : \x1b[0m" << cgi_extensions << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mCGI Bin : \x1b[0m" << cgi_bin << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mUpload Enabled : \x1b[0m" << (upload_enabled ? "\x1b[1;32mtrue\x1b[0m" : "\x1b[1;31mfalse\x1b[0m") << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mUpload Directory : \x1b[0m" << upload_directory << "\n";
    std::cout << "\x1b[1;33m║ \x1b[1;34mRedirect URL : \x1b[0m" << redirect_url << "\n";
}

Routes::~Routes() {}
//
// TODO: remove all comments from config file
