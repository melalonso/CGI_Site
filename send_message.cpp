#include <iostream>

using namespace std;

int main() {
    cout << "X-Frame-Options: DENY\n";
    cout << "Content-type:text/html\r\n\r\n";
    cout << "<html>" << endl;
    cout << "<head></head>" << endl;
    cout << "<body>" << endl;
    cout << "	<div id='header'><h1>Sitio de Ventas XYZ</h1></div>" << endl;
    cout << "	<div id='features'>" << endl;
    cout << "		<h3>Send a message</h3>" << endl;
    cout << "		<hr>" << endl;
    cout << "		<form action=\"/cgi-bin/send_message_aux\" method=\"post\" id='send_msg'>" << endl;
    cout << "			Full name: <input type=\"text\" name=\"name\" maxlength=\"30\" required><br>" << endl;
    cout << "           Message: <textarea name='message' rows=\"10\" cols=\"50\"></textarea>" << endl;
    cout << "			<input type=\"submit\" value=\"Send\">" << endl;
    cout << "		</form>" << endl;
    cout << "	</div>" << endl;
    cout << "</body>" << endl;
    cout << "<html>" << endl;
    return 0;
}
