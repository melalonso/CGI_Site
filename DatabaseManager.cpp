//
// Created by melalonso on 9/30/18.
//

#include "DatabaseManager.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

DatabaseManager::DatabaseManager() {
    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");
    con->setSchema("sitio_ventas");
}

void DatabaseManager::insert_product(std::string name, std::string description, int price) {
    std::cout << "Inserting product...\n";
}

void DatabaseManager::get_products() {
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * from products");
    while (res->next()) {
        int code = res->getInt(1);
        std::string name = res->getString(2);
        std::string description = res->getString(3);
        int price = res->getInt(4);
        code += price;

    }
}

//Definition for Destructor
DatabaseManager::~DatabaseManager() {
    delete res;
    delete stmt;
    delete con;
}


int main(){
    return 0;
}
