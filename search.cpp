#include <iostream>
using namespace std;

int main () {
   cout << "Content-type:text/html\r\n\r\n";
   cout << "<html>\n";
   cout << "<head>\n";
   cout << "<title>Search products</title>\n";
   cout << "</head>\n";
   cout << "<body>\n";
	cout << "<form action=\"/cgi-bin/results\" method=\"post\">" << endl;
	cout << "  <div class=\"container\">" << endl;
	cout << "    <label for=\"uname\"><b>Enter product</b></label>" << endl;
	cout << "    <input type=\"text\" placeholder=\"Enter product\" name=\"product\" required>" << endl;
	cout << "    <button type=\"submit\">Search</button>" << endl;
	cout << "  </div>" << endl;
	cout << "</form>" << endl;
   cout << "</body>\n";
   cout << "</html>\n";
   
   return 0;
}
