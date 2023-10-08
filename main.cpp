#include "./server/server.hpp"

int main(int ac, char **av) {
    if (ac != 2)
        return (1);
    Server server(av[1]);
    return 0;
}
