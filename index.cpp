#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>
#include <cstdlib>

using namespace std;

int main() {

    cout << "Content-type:text/html\r\n\r\n";

    if (const char *env_p = std::getenv("HTTP_COOKIE")) {
        cout << "Your PATH is: " << env_p << '\n';
        cout << "<h2> USUARIO LOGIEADO </h2>\n";
    }

    cout << "<html>\n"
            "<head></head>\n"
            "<body>\n"
            "\t<div id='header'><h1>Sitio de Ventas XYZ</h1></div>\n"
            "\t<div id='features'>\n"
            "\t\t<div id='search'>\n"
            "\t\t\t<form action=\"/cgi-bin/results\" method=\"post\">\n"
            "\t\t\t\t<div class=\"container\">\n"
            "\t\t\t\t\t<label for=\"product\"><b>Enter product</b></label>\n"
            "\t\t\t\t\t<input type=\"text\" placeholder=\"Enter product\" name=\"product\" required>\n"
            "\t\t\t\t\t<button type=\"submit\">Search</button>\n"
            "\t\t\t\t</div>\n"
            "\t\t\t</form>\n"
            "\t\t</div>\n"
            "\t\t<div id='login'>\n"
            "\t\t\t<h3>Users Login</h3>\n"
            "\t\t\t<form action=\"/cgi-bin/login\" method=\"post\">\n"
            "\t\t\t\t<div class=\"container\">\n"
            "\t\t\t\t\t<label for=\"username\"><b>Username</b></label>\n"
            "\t\t\t\t\t<input type=\"text\" placeholder=\"Enter username\" name=\"username\" required>\n"
            "\t\t\t\t\t<label for=\"password\"><b>Password</b></label>\n"
            "\t\t\t\t\t<input type=\"text\" placeholder=\"Enter password\" name=\"password\" required>\t\t\t\t\t\n"
            "\t\t\t\t\t<button type=\"submit\">Login</button>\n"
            "\t\t\t\t</div>\n"
            "\t\t\t</form>\t\t\t\n"
            "\t\t</div>\n"
            "\t\t<div id='register'>\n"
            "\t\t\t<h3><a href='/cgi-bin/register'>Users Register</a></h3>\t\t\n"
            "\t\t</div>\n"
            "\t</div>\n"
            "\t<div id='content'>\n";


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
    res = stmt->executeQuery("SELECT * from products");
    while (res->next()) {
        int code = res->getInt(1);
        string name = res->getString(2);
        string description = res->getString(3);
        int price = res->getInt(4);

        cout << "\t\t<div class='product' style='float:left;'>\n"
                "\t\t\t<p>Codigo: " << code << "</p>\n"
                                               "\t\t\t<p>Nombre: " << name << "</p>\n"
                                                                              "\t\t\t<p>Descripcion: " << description
             << "</p>\n"
                "\t\t\t<p>Precio: " << price << "</p>\n"
                                                "\t\t\t<button type=\"\">Add to cart</button>\n"
                                                "\t\t</div>\n";
    }


    cout << "\t</div>\n"
            "</body>\n"
            "</html>";


    delete res;
    delete stmt;
    delete con;

    return 0;
}