#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

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

string alphanum =
        "0123456789"
        "!@#$%^&*"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

string genRandomString(int size) {
    string randStr = "";
    while (size--) {
        randStr += alphanum[rand() % stringLength];
    }
    return randStr;
}

int main() {



    string login_data;
    cin >> login_data;
    map<string, string> parameters = parse(login_data);

    string username = parameters["username"];
    string password = parameters["password"];

    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");
    con->setSchema("sitio_ventas");

    stmt = con->createStatement();
    string query = "SELECT * from users where user_name =\"" + username + "\"";
    res = stmt->executeQuery(query);
    if (res->next()) {
        string stored_password = res->getString(6);

        if (password == stored_password) {
            string session_id = genRandomString(32);
            cout << "Set-Cookie: sid=" << session_id << "; Max-Age=3600; HttpOnly\n";
            cout << "Set-Cookie: cartid=" << 123456789 << "; Max-Age=3600; HttpOnly\n";
            cout << "Location: /cgi-bin/index\n";
            cout << "Content-type:text/html\r\n\r\n";
            //cout << "<script> document.location =  /cgi-bin/index ; </script>";
            cout << "Password is correct!\n";
            cout << "Logging in user...\n";
        } else {
            cout << "Content-type:text/html\r\n\r\n";
            cout << "Password is incorrect\n";
        }


        cout << "<a href='/cgi-bin/index'>Volver</a>";
    }

    delete res;
    delete stmt;
    delete con;

    return 0;
}
