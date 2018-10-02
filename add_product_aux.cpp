
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

int main() {

    string s = "";
    string line;
    while (cin >> line) {
        s = s + line;
    }
    map<string, string> parameters = parse(s);

    string name = parameters["name"];
    string description = parameters["description"];
    string price = parameters["price"];


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
            "INSERT INTO products(name, description, price) VALUES (?,?,?)");
    pstmt->setString(1, name);
    pstmt->setString(2, description);
    pstmt->setInt(3, stoi(price));
    pstmt->execute();
    delete pstmt;

    cout << "Content-type:text/html\r\n\r\n";
    cout << "<b>Lei</b>\n";
    for (auto &kv : parameters) {
        cout << kv.first << " = " << kv.second << "\n";
    }
    cout << "Inserted\n";
    cout << "<a href='/cgi-bin/index'>Volver</a>";

    // Eliminamos los objetos
    delete connection;


    return 0;
}
