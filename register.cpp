#include <iostream>
using namespace std;

int main () {
	cout << "Content-type:text/html\r\n\r\n";
	cout << "<html>" << endl;
	cout << "<head></head>" << endl;
	cout << "<body>" << endl;
	cout << "	<div id='header'><h1>Sitio de Ventas XYZ</h1></div>" << endl;
	cout << "	<div id='features'>" << endl;
	cout << "		<h3> Register new user</h3>" << endl;
	cout << "		<hr>" << endl;
	cout << "		<form action=\"/cgi-bin/register_aux\" method=\"post\">" << endl;
	cout << "			Full name: <input type=\"text\" name=\"name\" required><br>" << endl;
	cout << "			Username: <input type=\"text\" name=\"user\" required><br>" << endl;
	cout << "			Phone: <input type=\"text\" name=\"phone\" required><br>" << endl;
	cout << "			Email: <input type=\"text\" name=\"email\" required><br>" << endl;
	cout << "			Password: <input type=\"password\" name=\"password\" required><br>" << endl;
	cout << "			<input type=\"submit\" value=\"Register\">" << endl;
	cout << "		</form>" << endl;
	cout << "	</div>" << endl;
	cout << "</body>" << endl;
	cout << "<html>" << endl;
   	return 0;
}
