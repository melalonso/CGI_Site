#include <iostream>
#include <bits/stdc++.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <curl/curl.h>

using namespace std;

map<string, string> parse(const string &query) {
    string query2 = query.substr(0, query.size());
    map<string, string> data;
    regex pattern("([\\w+%]+)=([^&]*)");
    auto words_begin = sregex_iterator(query2.begin(), query2.end(), pattern);
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

int main() {
    cout << "Content-type:text/html\r\n\r\n";
    string input;
    cin >> input;
    map<string, string> parameters = parse(input);

    string name = decode(parameters["name"]);
    string user = decode(parameters["user"]);
    string phone = decode(parameters["phone"]);
    string email = decode(parameters["email"]);
    string password = decode(parameters["password"]);


    sql::Driver *driver;
    sql::Connection *connection;
    sql::PreparedStatement *pstmt;

    // Nos conectamos a nuestro servidor MySQL
    driver = get_driver_instance();
    connection = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");

    // Elegimos una base datos que ya tengamos creada ("prueba")
    connection->setSchema("sitio_ventas");


    /* '?' is the supported placeholder syntax */
    pstmt = connection->prepareStatement(
            "INSERT INTO users(full_name, user_name, email, phone, password) VALUES (?,?,?,?,?)");
    pstmt->setString(1, name);
    pstmt->setString(2, user);
    pstmt->setString(3, email);
    pstmt->setString(4, phone);
    pstmt->setString(5, password);
    pstmt->execute();
    delete pstmt;


    cout << "Usuario registrado de forma exitosa\n";
    cout << "<a href='/cgi-bin/index'>Volver</a>";

    // Eliminamos los objetos
    delete connection;


    return 0;
}
