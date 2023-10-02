#include "server.hpp"

Server::Server() {}

void    Server::SetServers(std::vector<ServerData>& svs) {
    it = svs.begin();
    while (it != svs.end()) {
        servers.push_back(*it);
        ++it;
    }
}

const std::vector<ServerData>&    Server::GetServers() const {
    return (servers);
}

void    Server::start() {
    ServerData  tmp;
    it = servers.begin();
    while (it != servers.end()) { // to change this shit code
        tmp = *it;
        tmp.print();
        tmp.initialization_and_socket_creation();
        tmp.bind_and_listen();
        tmp.select_accept_recv_send_handler();
        ++it;
    }
}

Server::~Server() {}
