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


class User {
public:
    int user_id;
    string full_name;
    string user_name;
    string email;
    string phone;
    string password;

    User() {}

    User(int user_id, string full_name, string user_name, string email, string phone, string password) {
        this->user_id = user_id;
        this->full_name = full_name;
        this->user_name = user_name;
        this->email = email;
        this->phone = phone;
        this->password = password;
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

class ShopCart {
public:
    string cookie_id;
    string product_id;
    int quantity;

    ShopCart(string cookie_id, string product_id, int quantity) {
        this->cookie_id = cookie_id;
        this->product_id = product_id;
        this->quantity = quantity;
    }
};


class DatabaseManager {
private:
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    sql::PreparedStatement *pstmt;
public:

    DatabaseManager() {
        /* Create a connection */
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "melalonso", "pass321");
        con->setSchema("sitio_ventas");
    }

    //Definition for Destructor
    ~DatabaseManager() {
        delete res;
        delete stmt;
        delete con;
        delete pstmt;
    }

    void insertUser(User u) {
        pstmt = connection->prepareStatement(
                "INSERT INTO users(full_name, user_name, email, phone, password) VALUES (?,?,?,?,?)");
        pstmt->setString(1, u.name);
        pstmt->setString(2, u.user);
        pstmt->setString(3, u.email);
        pstmt->setString(4, u.phone);
        pstmt->setString(5, u.password);
        pstmt->execute();
    }

    void updateUserCookie(string username, string cookieId) { // cookie of session
        stmt = connection->createStatement();
        stmt->executeUpdate("UPDATE users SET cookie_id = \"" + cookieId + "\" WHERE user_name = '" + username + "'");
    }

    void insertProduct(Product p) {
        pstmt = connection->prepareStatement(
                "INSERT INTO products(name, description, price) VALUES (?,?,?)");
        pstmt->setString(1, p.name);
        pstmt->setString(2, p.description);
        pstmt->setInt(3, p.price);
        pstmt->execute();
    }

    void insertShopCart(ShopCart sc) {
        pstmt = connection->prepareStatement(
                "INSERT INTO shop_cart(cookie_id, product_id, quantity) VALUES (?,?,?)");
        pstmt->setString(1, sc.cookie_id);
        pstmt->setString(2, sc.product_id);
        pstmt->setInt(3, sc.quantity);
        pstmt->execute();
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

    vector<Product> getProductsOnShoppingCart(string cookie_id) {
        vector<Product> results;
        stmt = con->createStatement();
        res = stmt->executeQuery("select * from shop_cart sc join products pr on sc.product_id=pr.product_id "
                                 "where sc.cookie_id='" + cookie_id + "';");
        while (res->next()) {
            string name = res->getString(5);
            string description = res->getString(6);
            int price = res->getInt(7);
            Product p(-1, name, description, price);
            results.push_back(p);
        }
        return results;
    }

    User getUser(string username) {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * from users where username=\"" + username + "\";");
        if (res->next()) {
            int user_id = res->getInt(1);
            string full_name = res->getString(2);
            string user_name = res->getString(3);
            string email = res->getString(4);
            string phone = res->getString(5);
            string password = res->getString(6);
            string cookie_id = res->getString(7);
            User user(user_id, full_name, email, phone, password, cookie_id);
        } else {
            User user = nullptr;
        }
        return user;
    }

    User getUserWithCookie(string cookie_id) {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * from users where cookie_id=\"" + cookie_id + "\";");
        if (res->next()) {
            int user_id = res->getInt(1);
            string full_name = res->getString(2);
            string user_name = res->getString(3);
            string email = res->getString(4);
            string phone = res->getString(5);
            string password = res->getString(6);
            string cookie_id = res->getString(7);
            User user(user_id, full_name, email, phone, password, cookie_id);
        } else {
            User user = nullptr;
        }
        return user;
    }


};


int main() {
    DatabaseManager dbMgr;
    bool isUserLogged = false;
    string ssid = "";

    if (const char *env_p = std::getenv("HTTP_COOKIE")) {
        cout << "Your sid is: " << env_p << '\n';
        ssid = string(env_p);
        isUserLogged = true;
    }

    if (isUserLogged) {
        User u = dbMgr.getUserWithCookie(ssid);
        dbMgr.updateUserCookie(u.username, '');
        cout << "Set-Cookie: sid=''; Expires=-1d; HttpOnly\n";
        cout << "Location: /cgi-bin/index\n";
        cout << "Content-type:text/html\r\n\r\n";
    } else {
        cout << "Content-type:text/html\r\n\r\n";
        cout << "Could not log out\n";
        cout << "<a href='/cgi-bin/index'>Volver</a>";
    }
    return 0;
}
