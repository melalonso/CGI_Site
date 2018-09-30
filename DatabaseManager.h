//
// Created by melalonso on 9/30/18.
//

#ifndef MYCGI_DATABASEMANAGER_H
#define MYCGI_DATABASEMANAGER_H

#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

class DatabaseManager {
private:
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
public:
    DatabaseManager();
    ~DatabaseManager();

    void insert_product(std::string, std::string, int);

    void get_products();


};


#endif //MYCGI_DATABASEMANAGER_H
