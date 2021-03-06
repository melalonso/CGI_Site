#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>
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
    string salt;
    string agent;
    string ip;

    User() {}

    User(int user_id, string full_name, string user_name, string email, string phone, string password,
         string cookie_id, string salt, string agent, string ip) {
        this->user_id = user_id;
        this->full_name = full_name;
        this->user_name = user_name;
        this->email = email;
        this->phone = phone;
        this->password = password;
        this->cookie_id = cookie_id;
        this->salt = salt;
        this->agent = agent;
        this->ip = ip;
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

class ShopCartProduct {
public:
    int quantity;
    int product_id;
    string name;
    string description;
    int price;

    ShopCartProduct(int product_id, string name, string description, int price, int quantity) {
        this->product_id = product_id;
        this->name = name;
        this->description = description;
        this->price = price;
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

    vector<ShopCartProduct> getProductsOnShoppingCart(int user_id) {
        vector<ShopCartProduct> results;
        pstmt = con->prepareStatement(
                "select * from shop_cart sc join products pr on sc.product_id=pr.product_id "
                "where sc.user_id=?;");
        pstmt->setInt(1, user_id);
        res = pstmt->executeQuery();
        while (res->next()) {
            string name = res->getString(6);
            string description = res->getString(7);
            int price = res->getInt(8);
            int quantity = res->getInt(3);
            ShopCartProduct p(-1, name, description, price, quantity);
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
            string salt = res->getString(8);
            string agent = res->getString(9);
            string ip = res->getString(10);
            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id, salt, agent, ip);
        }
        return user;
    }

    User *getUserWithCookie(string cookie_id) {
        User *user = nullptr;
        pstmt = con->prepareStatement("SELECT * from users where cookie_id=?;");
        pstmt->setString(1, cookie_id);
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
            string agent = res->getString(9);
            string ip = res->getString(10);
            user = new User(user_id, full_name, user_name, email, phone, password, cookie_id, salt, agent, ip);
        }
        return user;
    }


    bool isProductAlreadyOnCart(int userId, int productId) {
        pstmt = con->prepareStatement("select * from shop_cart where checkout=0 and user_id=?  and product_id=?");
        pstmt->setInt(1, userId);
        pstmt->setInt(2, productId);
        res = pstmt->executeQuery();
        return res->next() ? true : false;
    }

    void increaseQuantityCartProduct(int userId, int productId) {
        pstmt = con->prepareStatement("UPDATE shop_cart SET quantity = quantity + 1 where checkout=0 "
                                      "and user_id=? and product_id=?");
        pstmt->setInt(1, userId);
        pstmt->setInt(2, productId);
        pstmt->execute();
    }

    void setQuantityCartProduct(int userId, int productId, int quantity) {
        stmt = con->createStatement();
        stmt->executeUpdate("UPDATE shop_cart SET quantity =" + to_string(quantity) + " " +
                            "where checkout=0 and user_id=" +
                            to_string(userId) + " and product_id=" +
                            to_string(productId));
    }

    void insertCartProduct(int userId, int productId) {
        pstmt = con->prepareStatement(
                "INSERT INTO shop_cart(user_id, product_id, quantity, checkout) VALUES (?,?,?,?)");
        pstmt->setInt(1, userId);
        pstmt->setInt(2, productId);
        pstmt->setInt(3, 1);
        pstmt->setInt(4, 0);
        pstmt->execute();
    }


    void deleteProductFromCart(int userId, int productId) {
        stmt = con->createStatement();
        stmt->executeUpdate("delete from shop_cart where checkout=0 and "
                            "user_id=" + to_string(userId) + " and product_id=" + to_string(productId));
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


void printShopingCart(vector<ShopCartProduct> products) {
    int total = 0;
    cout << "<center>\n";
    cout << "<h3>Your Shopping Cart</h3>\n";
    cout << "<form action=\"/cgi-bin/checkout\" method=\"POST\">\n";
    cout << "<table>\n";
    cout << "\t<tr>\n";
    cout << "\t<th bgcolor=\"#cccccc\">Name</th>\n";
    cout << "\t<th bgcolor=\"#cccccc\">Description</th>\n";
    cout << "\t<th bgcolor=\"#cccccc\">Price</th>\n";
    cout << "\t<th bgcolor=\"#cccccc\">Quantity</th>\n";
    cout << "\t</tr>\n";
    int index = 0;
    for (auto product : products) {
        cout << "<tr>\n";
        cout << "<td align=\"CENTER\">" << product.name << "</td>\n";
        cout << "<td align=\"CENTER\">" << product.description << "</td>\n";
        cout << "<td align=\"CENTER\">" << product.price << "</td>\n";
        cout << "<td align=\"CENTER\"> <input type='text' name='qty" << index << "' value='" << product.quantity
             << "'></td>\n";
        total += product.quantity * product.price;
        cout << "</tr>\n";
        index++;
    }
    cout << "</table>\n";
    cout << "<b>Total to pay: " << total << "<br>\n";
    cout << "<input name=\"checkout\" type=\"submit\" value=\"Check Out\">\n"
            "<input name=\"checkout\" type=\"submit\" value=\"Update Quantities\">\n"
            "</form>\n"
            "<a href='/cgi-bin/index'>See more products</a>"
            "</center>\n";
}


bool checkUserLogged() {
    DatabaseManager *dbMgr2 = new DatabaseManager();
    string ssid = "";
    if (const char *env_p = std::getenv("HTTP_COOKIE")) {
        ssid = string(env_p);
        map<string, string> parameters = parse(ssid);
        if (!parameters.empty() && parameters["sid"] != "") {
            string agent = string(getenv("HTTP_USER_AGENT"));
            string ip = string(getenv("REMOTE_ADDR"));

            User *u = dbMgr2->getUserWithCookie(parameters["sid"]);
            if (u != nullptr) {
                if (u->agent == agent && u->ip == ip) {
                    string username = u->user_name;
                    cout << "<h2> Welcome " << username << " </h2>\n";
                    return true;
                }
            }
        }
    }
    return false;
}


bool isNumber(const std::string &s) {
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}


int main() {
    cout << "X-Frame-Options: DENY\n";
    cout << "Content-type:text/html\r\n\r\n";

    DatabaseManager *dbMgr = new DatabaseManager();
    string sid = "";
    int userId = -1;
    int productId = -1;
    bool isUserLogged = checkUserLogged();

    if (isUserLogged) {

        if (const char *env_p = std::getenv("HTTP_COOKIE")) {
            sid = string(env_p);
            map<string, string> parameters = parse(sid);
            if (!parameters.empty() && parameters["sid"] != "") {
                User *u = dbMgr->getUserWithCookie(parameters["sid"]);
                if (u != nullptr) {
                    userId = u->user_id;
                    string username = u->user_name;

                    if (const char *tmp = std::getenv("QUERY_STRING")) { // Receive item number
                        string queryString(tmp);
                        map<string, string> productRequested = parse(queryString);
                        if (!productRequested.empty() && productRequested["pid"] != "" &&
                            isNumber(productRequested["pid"])) {
                            productId = stoi(productRequested["pid"]);

                            if (dbMgr->isProductAlreadyOnCart(userId, productId)) {
                                dbMgr->increaseQuantityCartProduct(userId, productId);
                            } else {
                                dbMgr->insertCartProduct(userId, productId);
                            }

                        }
                    }

                    vector<ShopCartProduct> shoppingCartProducts = dbMgr->getProductsOnShoppingCart(userId);
                    printShopingCart(shoppingCartProducts);

                }
            }
        }
    } else {
        cout << "Log back in<br>";
        cout << "<a href='/cgi-bin/index'>Volver</a>";
    }

    return 0;
}