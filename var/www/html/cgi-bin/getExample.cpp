#include <iostream>

int main(int argc, char **argv, char **envp)
{
    std::cout << "<html>\n";
    std::cout << "<head>\n";
    std::cout << "<title>boobie</title>\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";
    std::cout << "<h2>Hello World! This is my first CGI program</h2>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";
    for (int i = 0; envp[i] != NULL; i++)
        std::cout << "<p>" << envp[i] << "</p>" << std::endl;
    std::cout << "<p>argc: " << argc << "</p>" << std::endl;
    std::cout << "<p>argv: " << argv[1] << "</p>" << std::endl;
    std::cout << "<p>argv: " << argv[2] << "</p>" << std::endl;

    return 0;
}