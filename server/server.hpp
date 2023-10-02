#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
# include "ConfigFileParser.hpp"

#define TRUE            1
#define FALSE           0
#define MAX_CONNECTIONS 42 //
#define PORT            8080 //

class Server
{
    public:
        Server();
        ~Server();

        void    SetServers(std::vector<ServerData>&);
        const std::vector<ServerData>& GetServers() const;

        void    start();
    private:
        std::vector<ServerData>             servers;
        std::vector<ServerData>::iterator   it;
};

#endif
