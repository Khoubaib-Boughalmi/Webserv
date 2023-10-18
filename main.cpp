#include "./server/server.hpp"
#include "server/Servers.hpp"

int main(int ac, char **av) {
    try {
        if (ac == 1) {
            char def[] = "config.conf";
            Server  Server(def);
            return (1);
        }
        else if (ac > 2) {
            throw Servers::ServerException("Too many arguments");
            return (1);
        }
        Server server(av[1]);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    // if (ac != 2) {
    //     char def[] = "config.conf";
    //     Server  Server(def);
    //     return (1);
    // }
    // Server server(av[1]);
    return 0;
}
