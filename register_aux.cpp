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
#include "bcrypt/BCrypt.hpp"

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
    string salt;

    User() {}

    User(int user_id, string full_name, string user_name, string email, string phone, string password,
         string cookie_id, string salt) {
        this->user_id = user_id;
        this->full_name = full_name;
        this->user_name = user_name;
        this->email = email;
        this->phone = phone;
        this->password = password;
        this->cookie_id = cookie_id;
        this->salt = salt;
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
                "INSERT INTO users(full_name, user_name, email, phone, password, salt) VALUES (?,?,?,?,?,?)");
        pstmt->setString(1, u.full_name);
        pstmt->setString(2, u.user_name);
        pstmt->setString(3, u.email);
        pstmt->setString(4, u.phone);
        pstmt->setString(5, u.password);
        pstmt->setString(6, u.salt);
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

//    User *getUser(string username) {
//        User *user = nullptr;
//        stmt = con->createStatement();
//        res = stmt->executeQuery("SELECT * from users where user_name=\"" + username + "\";");
//        if (res->next()) {
//            int user_id = res->getInt(1);
//            string full_name = res->getString(2);
//            string user_name = res->getString(3);
//            string email = res->getString(4);
//            string phone = res->getString(5);
//            string password = res->getString(6);
//            string cookie_id = res->getString(7);
//            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id);
//        }
//        return user;
//    }


    User *getUser(string username) {
        User *user = nullptr;
        pstmt = con->prepareStatement("SELECT * from users where user_name=?");
        pstmt->setString(1, username);
        res = pstmt->executeQuery();
        if (res->next()) {
            int user_id = res->getInt(1);
            string full_name = res->getString(2);
            string user_name = res->getString(3);
            string email = res->getString(4);
            string phone = res->getString(5);
            string password = res->getString(6);
            string cookie_id = res->getString(7);
            string salt = res->getString(8);
            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id, salt);
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
            string salt = res->getString(8);
            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id, salt);
        }
        return user;
    }


};

map<string, string> parse(const string &query) {
    string query2 = query.substr(0, query.size());
    map<string, string> data;
    regex pattern("([\\w+%]+)=([^&]*)");
    auto words_begin = sregex_iterator(query2.begin(), query2.end(), pattern);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; i++) {
        string key = (*i)[1].str();
        string value = (*i)[2].str();
        data[key] = value;
    }
    return data;
}

string decode(string src) {
    char a, b;
    int srcIndex = 0;
    string res;
    while (srcIndex < src.size()) {
        if ((src[srcIndex] == '%') &&
            ((a = src[srcIndex + 1]) && (b = src[srcIndex + 2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';

            if (b >= 'a') b -= 'a' - 'A';

            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';

            res += 16 * a + b;
            srcIndex += 3;
        } else if (src[srcIndex] == '+') {
            res += ' ';
            srcIndex++;
        } else {
            res += src[srcIndex];
            srcIndex++;
        }
    }
    return res;
}


string alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*abcdefghijklmnopqrstuvwxyz";

int stringLength = alphanumeric.length();

string genRandomString(int size) {
    string randomString = "";
    long seed1 = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed1);
    uniform_int_distribution<int> distribution(0, stringLength - 1);
    while (size--) {
        int randomCharIdx = distribution(generator);
        randomString += alphanumeric[randomCharIdx];
    }
    return randomString;
}

int main() {
    cout << "Content-type:text/html\r\n\r\n";
    string input;
    cin >> input;
    map<string, string> parameters = parse(input);

    string fullname = decode(parameters["name"]);
    string username = decode(parameters["user"]);
    string phone = decode(parameters["phone"]);
    string email = decode(parameters["email"]);
    string password = decode(parameters["password"]);


    DatabaseManager *dbMgr = new DatabaseManager();
    User *savedUser = dbMgr->getUser(username);
    if (savedUser == nullptr) {
        BCrypt bcrypt;
        string salt = genRandomString(32);
        string hash = bcrypt.generateHash(password + salt);
        User newUser = User(-1, fullname, username, email, phone, hash, "", salt);
        dbMgr->insertUser(newUser);
        cout << "User registered sucessfully<br>";
    } else {
        cout << "Username already taken. Choose another one<br>";
    }
    cout << "<a href='/cgi-bin/index'>Volver</a>";


    return 0;
}
