
#include <iostream>
#include <bits/stdc++.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

using namespace std;

map<string, string> parse(const string &query) {
    map<string, string> data;
    regex pattern("([\\w+%]+)=([^&]*)");
    auto words_begin = sregex_iterator(query.begin(), query.end(), pattern);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; i++) {
        string key = (*i)[1].str();
        string value = (*i)[2].str();
        data[key] = value;
    }
    return data;
}

string decode(string src) {
    char a, b;
    int srcIndex = 0;
    string res;
    while (srcIndex < src.size()) {
        if ((src[srcIndex] == '%') &&
            ((a = src[srcIndex + 1]) && (b = src[srcIndex + 2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';

            if (b >= 'a') b -= 'a' - 'A';

            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';

            res += 16 * a + b;
            srcIndex += 3;
        } else if (src[srcIndex] == '+') {
            res += ' ';
            srcIndex++;
        } else {
            res += src[srcIndex];
            srcIndex++;
        }
    }
    return res;
}


/* returns Str with all characters with special HTML meanings converted to
  entity references. */
string escapeHTML(string &Str) {
    string escaped = "";
    for (int i = 0; i < Str.size(); ++i) {
        string ThisCh = Str.substr(i, 1);
        if (ThisCh == "<")
            ThisCh = "&lt;";
        else if (ThisCh == ">")
            ThisCh = "&gt;";
        else if (ThisCh == "\"")
            ThisCh = "&quot;";
        else if (ThisCh == "'")
            ThisCh = "&apos;";
        else if (ThisCh == "&")
            ThisCh = "&amp;";
        escaped += ThisCh;
    }
    return escaped;
}

int main() {

    string s;
    cin >> s;
    map<string, string> parameters = parse(s);

    string name = decode(parameters["name"]);
    string description = decode(parameters["description"]);
    string price = decode(parameters["price"]);

    name = escapeHTML(name);
    description = escapeHTML(description);
    price = escapeHTML(price);

    sql::Driver *driver;
    sql::Connection *connection;
    sql::PreparedStatement *pstmt;

    // Nos conectamos a nuestro servidor MySQL
    driver = get_driver_instance();
    connection = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");

    // Elegimos una base datos que ya tengamos creada ("prueba")
    connection->setSchema("sitio_ventas");


    /* '?' is the supported placeholder syntax */
    pstmt = connection->prepareStatement("INSERT INTO products(name, description, price) VALUES (?,?,?)");
    pstmt->setString(1, name);
    pstmt->setString(2, description);
    pstmt->setInt(3, stoi(price));
    pstmt->execute();
    delete pstmt;

    cout << "X-Frame-Options: DENY\n";
    cout << "Content-type:text/html\r\n\r\n";
    cout << "Product was inserted<br>";
    cout << "<a href='/cgi-bin/index'>Volver</a>";

    // Eliminamos los objetos
    delete connection;


    return 0;
}
