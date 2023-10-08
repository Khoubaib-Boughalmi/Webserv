#include <iostream>
#include <map>
#include <string>
#include <fstream>

std::map<std::string, std::string> mimetypes;

void initMimeTypes() {
    mimetypes.insert(std::make_pair(".html", "text/html"));
    mimetypes.insert(std::make_pair(".css", "text/css"));
    mimetypes.insert(std::make_pair(".js", "application/javascript"));
    mimetypes.insert(std::make_pair(".jpg", "image/jpeg"));
    mimetypes.insert(std::make_pair(".jpeg", "image/jpeg"));
    mimetypes.insert(std::make_pair(".png", "image/png"));
    mimetypes.insert(std::make_pair(".gif", "image/gif"));
}

std::string process_and_load_file(std::string contentType) {
    std::string file;
    std::ifstream inputFile;
    
    if(contentType == "text/css")
        inputFile.open("./static/styles.css");
    else
        inputFile.open("./static/index.html");

    if (!inputFile.is_open()) {
        std::cout << "Error opening file" << std::endl;
        perror("Error: ");
        return "";
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        file += line;
        file += "\n\r";
    }
    inputFile.close();
    return file;
}

std::string determine_mime_type(std::string req) {
    initMimeTypes();
    std::string mime_type;
    std::string extension;
    size_t pos = req.find(".");
    std::string firstLine = req.substr(0, req.find(" \n\r\0"));
    if (pos != std::string::npos) {
        while (firstLine[pos] && firstLine[pos] != ' ' && firstLine[pos] != '\n' && firstLine[pos] != '\r')
        {
            extension += firstLine[pos];
            pos++;
        }
        mime_type = mimetypes[extension];
        if(mime_type.empty())
            mime_type = "text/html";
    }
    return mime_type;
}

