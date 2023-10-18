#include <iostream>

int main() {
    std::cout << "<!DOCTYPE html>\n";
    std::cout << "<html>\n" ;
    std::cout << "<head>\n" ;
    std::cout << "<title>HIIIIIIII</title>\n" ;
    std::cout << "<style>\n" ;
    std::cout << "body {\n" ;
    std::cout << "font-family: Arial, sans-serif;\n" ;
    std::cout << "background-color: #f1f1f1;\n" ;
    std::cout << "}\n" ;
    std::cout << "    \n" ;
    std::cout << ".container {\n" ;
    std::cout << "width: 300px;\n" ;
    std::cout << "margin: 0 auto;\n" ;
    std::cout << "margin-top: 100px;\n" ;
    std::cout << "padding: 20px;\n" ;
    std::cout << "background-color: #fff;\n" ;
    std::cout << "border: 1px solid #ccc;\n" ;
    std::cout << "border-radius: 5px;\n" ;
    std::cout << "}\n" ;
    
    std::cout << "h2 {\n";
    std::cout << "  text-align: center;\n";
    std::cout << "}\n";
    std::cout << "\n";
    std::cout << ".form-group {\n";
    std::cout << "  margin-bottom: 15px;\n";
    std::cout << "}\n";
    std::cout << "\n";
    std::cout << ".form-group label {\n";
    std::cout << "  display: block;\n";
    std::cout << "  margin-bottom: 5px;\n";
    std::cout << "}\n";
    std::cout << "\n";
    std::cout << ".form-group input {\n";
    std::cout << "  width: 100%;\n";
    std::cout << "  padding: 5px;\n";
    std::cout << "  border: 1px solid #ccc;\n";
    std::cout << "  border-radius: 3px;\n";
    std::cout << "}\n";
    std::cout << "\n";
    std::cout << ".form-group button {\n";
    std::cout << "  width: 100%;\n";
    std::cout << "  padding: 8px 12px;\n";
    std::cout << "  background-color: #4CAF50;\n";
    std::cout << "  border: none;\n";
    std::cout << "  color: #fff;\n";
    std::cout << "  cursor: pointer;\n";
    std::cout << "  border-radius: 3px;\n";
    std::cout << "}\n";
    std::cout << "</style>\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";
    std::cout << "<div class='container'>\n";
    std::cout << "<h2>HELLO WORLD</h2>\n";
    std::cout << "<form action='/cgi-bin/post' method='POST'>\n";
    std::cout << "<div class='form-group'>\n";
    std::cout << "<label for='username'>Username:</label>\n";
    std::cout << "<input type='text' id='username' name='username' required>\n";
    std::cout << "</div>\n";
    std::cout << "<div class='form-group'>\n";
    std::cout << "<label for='password'>Password:</label>\n";
    std::cout << "<input type='password' id='password' name='password' required>\n";
    std::cout << "</div>\n";
    std::cout << "<div class='form-group'>\n";
    std::cout << "<button type='submit'>TEEEEEST</button>\n";
    std::cout << "</div>\n";
    std::cout << "</form>\n";
    std::cout << "</div>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";
    
}
