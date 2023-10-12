# pragma once

#include <string>
#include <map>
#include <sys/socket.h>

class Response {
public:
    Response();
    ~Response();

    Response& setStatus(int statusCode);

    Response& setBody(const std::string& responseBody);

    Response& setContentType(const std::string& contentType);

    Response& setCookie(const std::string& cookie) ;

    Response& setLocation(const std::string& location) ;
    
    void sendResponse(const int) const;
    std::string getStatusMessage(int statusCode) const;

private:
    int statusCode;
    std::string responseBody;
    std::string contentType;
    std::string cookie;
    std::string location;
};

const std::string getMimeType(const std::string& fileExtension);
