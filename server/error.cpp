#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<int, std::string> errorPages;

    errorPages[400] = "<html><head><title>400 Bad Request</title></head><body><h1>400 Bad Request</h1><p>Your request could not be understood or was missing required parameters.</p></body></html>";

    errorPages[401] = "<html><head><title>401 Unauthorized</title></head><body><h1>401 Unauthorized</h1><p>You must authenticate to access this resource.</p></body></html>";

    errorPages[403] = "<html><head><title>403 Forbidden</title></head><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";

    errorPages[404] = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body></html>";

    errorPages[405] = "<html><head><title>405 Method Not Allowed</title></head><body><h1>405 Method Not Allowed</h1><p>The request method is not supported for this resource.</p></body></html>";

    errorPages[413] = "<html><head><title>413 Request Entity Too Large</title></head><body><h1>413 Request Entity Too Large</h1><p>The request entity exceeds the server's limits.</p></body></html>";

    errorPages[414] = "<html><head><title>414 URI Too Long</title></head><body><h1>414 URI Too Long</h1><p>The requested URI is too long and cannot be processed.</p></body></html>";

    errorPages[409] = "<html><head><title>409 Conflict</title></head><body><h1>409 Conflict</h1><p>A conflict occurred while processing the request.</p></body></html>";

    errorPages[501] = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1><p>The server does not support the requested feature.</p></body></html>";

    errorPages[301] = "<html><head><title>301 Moved Permanently</title></head><body><h1>301 Moved Permanently</h1><p>The requested resource has been permanently moved to a new location.</p></body></html>";

    errorPages[200] = "<html><head><title>200 OK</title></head><body><h1>200 OK</h1><p>The request was successful.</p></body></html>";

    for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "Status Code: " << it->first << "\n" << it->second << "\n\n";
    }

    return (0);
}
