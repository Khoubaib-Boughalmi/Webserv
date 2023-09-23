#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <sstream>
#include <vector>

#define PORT 8086

class Request
{
public:
    std::string method;
    std::string path;
    std::string httpVersion;
    std::string ContentType;
    std::string Content;
    void parse(std::string str)
    {

        // Find the position of the first space
        size_t firstSpacePos = str.find(' ');

        // Find the position of the second space, starting from the position after the first space
        size_t secondSpacePos = str.find(' ', firstSpacePos + 1);

        // Extract the HTTP method (e.g., "GET")
        this->method = str.substr(0, firstSpacePos);

        // Extract the path (e.g., "/favicon.ico")
        this->path = str.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
        //     std::cout << "path" << this->path << std::endl;
        // if (this->path.find("css")!=std::string::npos)
        //     std::cout << "found css" << std::endl;
        std::string actualPath = "website" + this->path;
        
        if (this->path == "/" && this->path.length()==1)
        {
            this->ContentType = "text/html";
            this->path = "/file.html";
            this->Content = readHtmlFile("website/file.html");
        }
        else if (this->path.find(".css")!=std::string::npos)
        {
            this->ContentType = "text/css";
            this->Content = readHtmlFile(actualPath);
            std::cout <<this->Content << std::endl;
        }
        else if (this->path.find(".js")!=std::string::npos)
        {
            this->ContentType = "application/javascript";
            this->Content = readHtmlFile(actualPath);
            std::cout <<this->Content << std::endl;
        }
        else if (this->path.find(".jpg")!=std::string::npos)
        {
            this->ContentType = "image/jpeg";
            this->Content = readImageFile(actualPath);
        }
        else
        {
            this->ContentType = "text/html";
            this->Content = readHtmlFile(actualPath);
        }

        // Extract the HTTP version (e.g., "HTTP/1.1")
        this->httpVersion = str.substr(secondSpacePos + 1);
    }

    std::string readImageFile(const std::string &filename)
    {
        std::ifstream file(filename.c_str(), std::ios::binary);
        if (!file.is_open())
        {
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string readHtmlFile(const std::string &filename)
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
};

int main(int argc, char **argv)
{
    int port = PORT;
    if (argc == 2)
        port = atoi(argv[1]);
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port); // port number
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 10) == -1)
    {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port 8080..." << std::endl;
    std::cout << "http://localhost:" << port << std::endl;

    while (true)
    {
        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == -1)
        {
            perror("Accepting client connection failed");
            continue;
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // Read the HTTP request from the client
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0)
        {
            perror("Error reading request");
            close(clientSocket);
            continue;
        }
        // std::cout << buffer << std::endl;
        std::string strBuffer = buffer;
        Request req;
        strBuffer = strBuffer.substr(0, strBuffer.find("\n"));
        std::cout << "strBuffer"  << strBuffer << std::endl;
        req.parse(strBuffer);

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: ";
        response += req.ContentType;
        response += "\r\nContent-Length: ";

        // // Create an HTTP response with the HTML content

        response += std::to_string(req.Content.length());
        response += "\r\n\r\n";
        response += req.Content;
        std::cout <<"!!" <<response<<"!!" << std::endl<< std::endl<< std::endl;
        // response += htmlContent;

        // strcat(response, std::to_string(strlen(htmlContent.c_str())).c_str());
        // std::cout << "html" << htmlContent <<std::endl;

        // std::cout << "----\n"
        //           << response << "++++\n"
        //           << std::endl;
        // const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 68\r\n\r\n<html><body><h1 style='color: red;'>Hello, World!</h1></body></html>";
        // std::cout <<"'" <<response <<"'"<< std::endl;

        send(clientSocket, response.c_str(), response.length(), 0);

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket (not reached in this example)
    close(serverSocket);

    return (0);
}
