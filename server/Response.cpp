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

Response& Response::setCookie(const std::string& cookie) {
    this->cookie = cookie;
    return *this;
}

Response& Response::setLocation(const std::string& location) {
    this->location = location;
    return *this;
}

std::string Response::getStatusMessage(int statusCode) const {
    switch (statusCode) {
        case 200: return " OK";
        case 201: return " Created";
        case 204: return " No Content";
        case 301: return " Moved Permanently";
        case 400: return " Bad Request";
        case 403: return " Forbidden";
        case 404: return " Not Found";
        case 405: return " Method Not Allowed";
        case 409: return " Conflict";
        case 413: return " Request Entity Too Large";
        case 414: return " Request-URI Too Long";
        case 500: return " Internal Server Error";
        case 501: return " Not Implemented";
        default:  return "CGI STATUS";
    }
}

int Response::sendResponse(const int clientFD) const {
    std::string response;
    response = "HTTP/1.1 ";
    response.append(std::to_string(statusCode));
    response.append(getStatusMessage(statusCode));
    response.append("\r\n");
    response.append("Cache-Control: no-cache, no-store, must-revalidate\r\n");
    response.append("Pragma: no-cache\r\n");
    response.append("Expires: 0\r\n");
    response.append("Location: ");
    response.append(location);
    response.append("\r\n");
    response.append("Content-Length: ");
    response.append(std::to_string(responseBody.size()));
    response.append("\r\n");
    response.append("set-cookie: ");
    response.append(cookie);
    response.append("\r\n");
    response.append("Content-Type: ");
    response.append(contentType);
    response.append("\r\n\r\n");
    response.append(responseBody);
    return (send(clientFD, response.c_str() , response.length() , 0));
}

std::map<std::string, std::string>  _mimeTypes;
void initializeMimeTypes() {
    _mimeTypes["html"] = "text/html";
    _mimeTypes["htm"] = "text/html";
    _mimeTypes["txt"] = "text/plain";
    _mimeTypes["jpg"] = "image/jpeg";
    _mimeTypes["png"] = "image/png";
    _mimeTypes["zip"] = "application/zip";

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
