#include <iostream>
using namespace std;

string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}


int main() {
	string line;
	while (getline(cin, line)){
		line = ReplaceString(line, "\"", "\\\"");
		cout << "cout << \""<<line<<"\" << endl;"<< endl;
	}
	return 0;
}