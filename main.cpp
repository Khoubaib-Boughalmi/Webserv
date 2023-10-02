#include "./server/server.hpp"
# include "server/ConfigFileParser.hpp"

int main(int ac, char **av) {
    if (ac != 2) {
        std::cerr << "error" << std::endl;
        return (1);
    }
    // std::string file = av[1];
    std::vector<std::string> servers;
    int servers_nb = 0;
    std::string conf;
    read_file(av[1], conf, servers_nb);
    parse_file(conf, servers_nb, servers);
    std::vector<std::string>::iterator  it = servers.begin();
    std::vector<ServerData> svs;
    while (it != servers.end()) {
        fill_class(*it, svs);
        ++it;
    }

    Server server;
    server.SetServers(svs);
    server.start();
    return (0);
}
