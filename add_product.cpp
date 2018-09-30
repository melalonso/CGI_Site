#include <iostream>
using namespace std;

int main () {
    cout << "Content-type:text/html\r\n\r\n";
    cout << "<html>" << endl;
    cout << "<head></head>" << endl;
    cout << "<body>" << endl;
    cout << "	<div id='header'><h1>Sitio de Ventas XYZ</h1></div>" << endl;
    cout << "	<div id='features'>" << endl;
    cout << "		<h3> Sell product </h3>" << endl;
    cout << "		<hr>" << endl;
    cout << "		<form action=\"/cgi-bin/add_product_aux\" method=\"post\">" << endl;
    cout << "			Name: <input type=\"text\" name=\"name\" required><br>" << endl;
    cout << "			Description: <input type=\"text\" name=\"description\" required><br>" << endl;
    cout << "			Price: <input type=\"text\" name=\"price\" required><br>" << endl;
    cout << "			<input type=\"submit\" value=\"Add\">" << endl;
    cout << "		</form>" << endl;
    cout << "	</div>" << endl;
    cout << "</body>" << endl;
    cout << "<html>" << endl;
    return 0;
}
