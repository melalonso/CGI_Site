#include <bits/stdc++.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>

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

    string search_text;
    cin >> search_text;
    map<string, string> parameters = parse(search_text);

    cout << "Content-type:text/html\r\n\r\n";

    // for (auto& t : parameters) cout << t.first << " " << t.second << "\n";

    search_text = parameters["product"];

    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");
    /* Connect to the MySQL test database */
    con->setSchema("sitio_ventas");

    stmt = con->createStatement();
    string query = "SELECT * from products where name like \"%" + search_text + "%\"";
    res = stmt->executeQuery(query);
    while (res->next()) {
        cout << res->getInt(1) << endl;
        cout << res->getString(2) << endl;
        cout << res->getString(3) << endl;
        cout << res->getInt(4) << endl;
    }

    delete res;
    delete stmt;
    delete con;

    return 0;
}

