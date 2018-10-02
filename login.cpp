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
#include <cppconn/prepared_statement.h>

using namespace std;


class User {
public:
    int user_id;
    string full_name;
    string user_name;
    string email;
    string phone;
    string password;
    string cookie_id;

    User() {}

    User(int user_id, string full_name, string user_name, string email, string phone, string password,
         string cookie_id) {
        this->user_id = user_id;
        this->full_name = full_name;
        this->user_name = user_name;
        this->email = email;
        this->phone = phone;
        this->password = password;
        this->cookie_id = cookie_id;
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
        pstmt = con->prepareStatement(
                "INSERT INTO users(full_name, user_name, email, phone, password) VALUES (?,?,?,?,?)");
        pstmt->setString(1, u.full_name);
        pstmt->setString(2, u.user_name);
        pstmt->setString(3, u.email);
        pstmt->setString(4, u.phone);
        pstmt->setString(5, u.password);
        pstmt->execute();
    }

    void updateUserCookie(string username, string cookieId) { // cookie of session
        stmt = con->createStatement();
        stmt->executeUpdate("UPDATE users SET cookie_id = \"" + cookieId + "\" WHERE user_name = '" + username + "'");
    }

    void insertProduct(Product p) {
        pstmt = con->prepareStatement(
                "INSERT INTO products(name, description, price) VALUES (?,?,?)");
        pstmt->setString(1, p.name);
        pstmt->setString(2, p.description);
        pstmt->setInt(3, p.price);
        pstmt->execute();
    }

    void insertShopCart(ShopCart sc) {
        pstmt = con->prepareStatement(
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

    User *getUser(string username) {
        User *user = nullptr;
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * from users where user_name=\"" + username + "\";");
        if (res->next()) {
            int user_id = res->getInt(1);
            string full_name = res->getString(2);
            string user_name = res->getString(3);
            string email = res->getString(4);
            string phone = res->getString(5);
            string password = res->getString(6);
            string cookie_id = res->getString(7);
            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id);
        }
        return user;
    }

    User *getUserWithCookie(string cookie_id) {
        User *user = nullptr;
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
            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id);
        }
        return user;
    }


};

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

string alphanum = "0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

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


    DatabaseManager *dbMgr = new DatabaseManager();

    User *u = dbMgr->getUser(username);

    if (u != nullptr) {
        string stored_password = u->password;
        if (password == stored_password) {
            string session_id = genRandomString(32);
            cout << "Set-Cookie: sid=" << session_id << "; Max-Age=3600; HttpOnly\n";
            dbMgr->updateUserCookie(username, session_id);
            cout << "Location: /cgi-bin/index\n";
            cout << "Content-type:text/html\r\n\r\n";
            cout << "Password is correct!\n";
            cout << "Logging in user...\n";
        } else {
            cout << "Content-type:text/html\r\n\r\n";
            cout << password << " vs " << stored_password << "\n";
            cout << (password == stored_password) << "<br>\n";
            cout << u->user_name << " - " << u->email << "<br>\n\n";
            cout << (password.compare(stored_password) == 0) << "<br>\n";
            cout << (stored_password==stored_password) << " ---- \n";
            cout << typeid(password).name() << "<br>";
            cout << typeid(stored_password).name() << "<br>";
            cout << "Password is incorrect\n";
        }
        cout << "<a href='/cgi-bin/index'>Volver</a>";
    }

    delete u;
    delete dbMgr;
    return 0;
}
