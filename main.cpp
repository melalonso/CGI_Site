#include <iostream>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>
#include <cstdlib>

using namespace std;


class Product {
public:
    int product_id;
    string name;
    string description;
    int price;

    Product(int product_id, string name, string description, int price) {
        product_id = product_id;
        name = name;
        description = description;
        price = price;
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

    vector <Product> get_products() {
        vector <Product> results;
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

int main() {
    DatabaseManager db;

    vector<Product> v = db.get_products();
    for (auto i : v){
        cout << i.name << "\n";
    }
    return 0;
}