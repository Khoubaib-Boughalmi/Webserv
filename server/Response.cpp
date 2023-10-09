#include "Response.hpp"
#include <iostream>

Response::Response() {}

Response& Response::setStatus(int statusCode) {
    this->statusCode = statusCode;
    return *this;
}

Response& Response::setBody(const std::string& responseBody) {
    this->responseBody = responseBody;
    return *this;
}

Response& Response::setContentType(const std::string& contentType) {
    this->contentType = contentType;
    return *this;
}

std::string Response::getStatusMessage(int statusCode) const {
    switch (statusCode) {
        case 200: return " OK";
        case 400: return " Bad Request";
        case 404: return " Not Found";
        case 500: return " Internal Server Error";
        default:  return " Unknown Status";
    }
}

void Response::sendResponse(const int clientFD) const {
    std::string response;
    response = "HTTP/1.1 ";
    response.append(std::to_string(statusCode));
    response.append(getStatusMessage(statusCode));
    response.append("\r\n");
    response.append("Content-Length: ");
    response.append(std::to_string(responseBody.size()));
    response.append("\r\n");
    response.append("Content-Type: ");
    response.append(contentType);
    response.append("\r\n\r\n");
    response.append(responseBody);
    // std::cout << response << std::endl;
    send(clientFD, response.c_str() , response.length() , 0);
}

std::map<std::string, std::string>  _mimeTypes;
void initializeMimeTypes() {
    _mimeTypes["html"] = "text/html";
    _mimeTypes["htm"] = "text/html";
    _mimeTypes["txt"] = "text/plain";
    _mimeTypes["jpg"] = "image/jpeg";
    _mimeTypes["jpeg"] = "image/jpeg";
    _mimeTypes["png"] = "image/png";
}

const std::string getMimeType(const std::string& fileExtension) {
    static bool initialized = false;
    if (!initialized) {
        initializeMimeTypes();
        initialized = true;
    }

    std::map<std::string, std::string>::iterator it = _mimeTypes.find(fileExtension);
    if (it != _mimeTypes.end()) {
        return it->second;
    } else {
        return "application/octet-stream";
    }
}

Response::~Response() {}
