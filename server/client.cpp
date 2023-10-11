#include "client.hpp"

Client::Client():  disconnected(0), clientFD(0), serverFD(0), connected_time(0)
{}

Client::Client(int clientFD, int serverFD, Servers& server)
{
    this->disconnected = 0;
    this->clientFD = clientFD;
    this->serverFD = serverFD;
    this->server = server;
}

Client::~Client()
{
}

Client::Client(const Client &copy) {
    *this = copy;
}

Client &Client::operator=(const Client &copy) {
    if (this != &copy) {
        this->disconnected = copy.disconnected;
        this->clientFD = copy.clientFD;
        this->serverFD = copy.serverFD;
        this->address = copy.address;
        this->connected_time = copy.connected_time;
        this->server = copy.server;
    }

    return *this;
}

void Client::update_connected_time(void) {
    this->connected_time = std::time(0);
}