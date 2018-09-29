#include <iostream>
using namespace std;

int main () {
   cout << "Content-type:text/html\r\n\r\n";
   cout << "<html>\n";
   cout << "<head>\n";
   cout << "<title>\"Hello World - First CGI Program</title>\n";
   cout << "</head>\n";
   cout << "<body>\n";
   cout << "<form action=\"action_page.php\" method=\"post\">" << endl;
   cout << "  <div class=\"container\">" << endl;
   cout << "    <label for=\"uname\"><b>Username</b></label>" << endl;
   cout << "    <input type=\"text\" placeholder=\"Enter Username\" name=\"uname\" required>" << endl;
   cout << "    <label for=\"psw\"><b>Password</b></label>" << endl;
   cout << "    <input type=\"password\" placeholder=\"Enter Password\" name=\"psw\" required>" << endl;
   cout << "    <button type=\"submit\">Login</button>" << endl;
   cout << "  </div>" << endl;
   cout << "</form>" << endl; 
   cout << "</body>\n";
   cout << "</html>\n";
   
   return 0;
}
