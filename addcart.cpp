//
// Created by melalonso on 9/30/18.
//

#include "addcart.h"
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>
#include <cstdlib>

using namespace std;

class User {
public:
    int user_id;
    string full_name;
    string user_name;
    string email;
    string phone;
    string password;

    User(int user_id, string full_name, string user_name, string email, string phone, string password) {
        user_id = user_id;
        full_name = full_name;
        user_name = user_name;
        email = email;
        phone = phone;
        password = password;
    }
};

class Product {
public:
    int product_id;
    string name;
    string description;
    int price;

    Product(int product_id, string name, string description, int price) {
        this->product_id = product_id;
        this->name = name;
        this->description = description;
        this->price = price;
    }
};


class DatabaseManager {
private:
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
public:

    DatabaseManager() {
        /* Create a connection */
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");
        con->setSchema("sitio_ventas");
    }

    void insert_product(std::string name, std::string description, int price) {
        std::cout << "Inserting product...\n";
    }

    vector<Product> getProducts() {
        vector<Product> results;
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * from products");
        while (res->next()) {
            int product_id = res->getInt(1);
            std::string name = res->getString(2);
            std::string description = res->getString(3);
            int price = res->getInt(4);
            Product p(product_id, name, description, price);
            results.push_back(p);
        }
        return results;
    }

//Definition for Destructor
    ~DatabaseManager() {
        delete res;
        delete stmt;
        delete con;
    }
};


void print_products(vector<Product> products, bool isUserLogged) {
    for (auto product : products) {
        cout << "\t\t<div class='product' style='float:left;'>\n";
        cout << "\t\t\t<p>Codigo: " << product.product_id << "</p>\n";
        cout << "\t\t\t<p>Nombre: " << product.name << "</p>\n";
        cout << "\t\t\t<p>Descripcion: " << product.description << "</p>\n";
        cout << "\t\t\t<p>Precio: " << product.price << "</p>\n";
        if (isUserLogged) {
            cout << "<a href=\"/cgi-bin/addcart?" << product.product_id << "\">Add to Cart</a><br><br>\n";
        }
        cout << "\t\t</div>\n";
    }
}


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


void printShopingCart(vector<Product> products) {
    cout << "<table>\n";
    cout << "\t<tr>\n";
    cout << "\t<th>Codigo</th>\n";
    cout << "\t<th>Nombre</th>\n";
    cout << "\t<th>Descripcion</th>\n";
    cout << "\t<th>Precio</th>\n";
    cout << "\t</tr>\n";
    for (auto product : products) {
        cout << "<tr>\n";
        cout << "<td>" << product.product_id << "</td>\n";
        cout << "<td>" << product.name << "</td>\n";
        cout << "<td>" << product.description << "</td>\n";
        cout << "<td>" << product.price << "</td>\n";
        cout << "</tr>\n";
    }
    cout << "</table>\n";
}


int main() {

    if (const char *tmp = std::getenv("QUERY_STRING")) { // Receive item number
        string queryString(tmp);
        map<string, string> parameters = parse(queryString);

    }

    DatabaseManager dbMgr;

    cout << "Content-type:text/html\r\n\r\n";
    bool isUserLogged = false;

    if (const char *env_p = std::getenv("HTTP_COOKIE")) {
        cout << "Your sid is: " << env_p << '\n';
        cout << "<h2> USUARIO LOGIEADO </h2>\n";
        isUserLogged = true;
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
            "\t</div>\n";

    if (isUserLogged) {
        cout << "\t\t<div id='add_product'>\n"
                "\t\t\t<h3><a href='/cgi-bin/add_product'>Sell product</a></h3>\n"
                "\t\t</div>";
    }

    cout << "\t<div id='content'>\n";

    vector<Product> products = dbMgr.getProducts();
    print_products(products, isUserLogged);
    cout << "\t</div>\n"
            "</body>\n"
            "</html>";

    return 0;
}