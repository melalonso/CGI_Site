#include <iostream>
using namespace std;

int main (int argc, char**argv) {
	//data = getenv("QUERY_STRING");
	string s = "";
	string line;
	while (cin>>line){
		s = s + line + "\n";
	}
	cout << "Content-type:text/html\r\n\r\n";
	cout << "<b>Lei</b>\n";
	cout << s << "\n";
   	return 0;
}
