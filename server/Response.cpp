#include <iostream>
#include <string>
#include <map>

class Response {
public:
    Response& setStatus(int statusCode) {
        this->statusCode = statusCode;
        return *this;
    }

    Response& setBody(const std::string& responseBody) {
        this->responseBody = responseBody;
        return *this;
    }

    Response& setContentType(const std::string& contentType) {
        this->contentType = contentType;
        return *this;
    }

    void sendResponse() const {
        std::cout << "HTTP/1.1 " << statusCode << " " << getStatusMessage(statusCode) << "\r\n";
        std::cout << "Content-Length: " << responseBody.size() << "\r\n";
        std::cout << "Content-Type: " << contentType << "\r\n\r\n";
        std::cout << responseBody << std::endl;
    }

private:
    int statusCode;
    std::string responseBody;
    std::string contentType;

    std::string getStatusMessage(int statusCode) const {
        switch (statusCode) {
            case 200: return "OK";
            case 400: return "Bad Request";
            case 404: return "Not Found";
            case 500: return "Internal Server Error";
            default:  return "Unknown Status";
        }
    }
};

std::map<std::string, std::string> mimeTypes;

void initializeMimeTypes() {
    mimeTypes["html"] = "text/html";
    mimeTypes["htm"] = "text/html";
    mimeTypes["txt"] = "text/plain";
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["png"] = "image/png";
}

const std::string getMimeType(const std::string& fileExtension) {
    static bool initialized = false;
    if (!initialized) {
        initializeMimeTypes();
        initialized = true;
    }

    std::map<std::string, std::string>::iterator it = mimeTypes.find(fileExtension);
    if (it != mimeTypes.end()) {
        return it->second;
    } else {
        return "application/octet-stream";
    }
}

int main() {
    Response response;

    // Set response status code, body, and content type using chaining
    response.setStatus(200).setBody("Hello, World!").setContentType(getMimeType("html"));

    // Send the HTTP response
    response.sendResponse();

    return 0;
}
