#ifndef CLIENT_HPP
#define CLIENT_HPP

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
#include <ctime>

class Client
{
    private:
        /* data */
    public:
        Client();
        Client(int clientFD, int serverFD);
        Client(const Client &copy);
        Client &operator=(const Client &copy);
        ~Client();
        void update_connected_time(void);
        int disconnected;
        int clientFD;
        int serverFD;
        std::time_t connected_time;
        struct sockaddr_in address;
};

#endif