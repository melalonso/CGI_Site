#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>
#include <cstdlib>
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


class Order {
public:
    int orderId;
    int userId;
    string shippingAddress;
    string city;
    string state;
    string country;

    Order() {}

    Order(int orderId, int userId, string shippingAddress, string city, string state, string country) {
        this->orderId = orderId;
        this->userId = userId;
        this->shippingAddress = shippingAddress;
        this->city = city;
        this->state = state;
        this->country = country;
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

    vector<Product> getProductsOnShoppingCart(int user_id) {
        vector<Product> results;
        stmt = con->createStatement();
        res = stmt->executeQuery("select * from shop_cart sc join products pr on sc.product_id=pr.product_id "
                                 "where sc.user_id=" + to_string(user_id) + ";");
        while (res->next()) {
            string name = res->getString(6);
            string description = res->getString(7);
            int price = res->getInt(8);
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


    bool isProductAlreadyOnCart(int userId, int productId) {
        string query = "select * from shop_cart where checkout=0 and "
                       "user_id=" + to_string(userId) + " and product_id=" + to_string(productId) + ";";
        res = stmt->executeQuery(query);
        return res->next() ? true : false;
    }

    void increaseQuantityCartProduct(int userId, int productId) {
        stmt = con->createStatement();
        stmt->executeUpdate("UPDATE shop_cart SET quantity = quantity + 1 "
                            "where checkout=0 and user_id=" + to_string(userId) + " and product_id=" +
                            to_string(productId));
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


    void insertOrder(Order o) {
        pstmt = con->prepareStatement(
                "INSERT INTO orders(user_id, shipping_address, city, state, country) VALUES (?,?,?,?,?)");
        pstmt->setInt(1, o.userId);
        pstmt->setString(2, o.shippingAddress);
        pstmt->setString(3, o.city);
        pstmt->setString(4, o.state);
        pstmt->setString(5, o.country);
        pstmt->execute(); // OCUPO EL ORDER ID
    }

    void insertOrderProduct(int orderId, int productId) {
        pstmt = con->prepareStatement(
                "INSERT INTO order_products(order_id, user_id) VALUES (?,?)");
        pstmt->setInt(1, orderId);
        pstmt->setInt(2, productId);
        pstmt->execute();
    }

    void cleanShopCart(int userId) {
        stmt = con->createStatement();
        stmt->executeUpdate("delete from shop_cart where user_id=" + to_string(userId));
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


void printOrderSummary(vector<Product> products, Order order) {
    int total = 0;
    int subTotal = 0;
    cout << "<center>\n";
    cout << "<h2>Thank you!</h2>\n";
    cout << "<p>Shipping address: " << order.shippingAddress << "</p>";
    cout << "<p>City: " << order.city << "</p>";
    cout << "<p>State: " << order.state << "</p>";
    cout << "<p>Country: " << order.country << "</p>";

    cout << "<h5>Products: </h5>\n";
    cout << "<ol>\n";
    for (auto product : products) {
        cout << "<li>" << product.name << ": $" << product.price << "</li>\n";
        subTotal += product.price;
    }
    cout << "</ol>\n";
    total = subTotal + (subTotal * 0.10)
    cout << "<b>Subtotal: " << subTotal << "<br>\n";
    cout << "<b>Total a pagar (subtotal + shipping): " << total << "<br>\n";
}


int main() {
    cout << "Content-type:text/html\r\n\r\n";

    DatabaseManager *dbMgr = new DatabaseManager();
    string sid = "";
    int userId = -1;
    int productId = -1;
    bool isUserLogged = false;

    if (const char *env_p = std::getenv("HTTP_COOKIE")) {
        sid = string(env_p);
        map<string, string> parameters = parse(sid);
        if (!parameters.empty() && parameters["sid"] != "") {
            User *u = dbMgr->getUserWithCookie(parameters["sid"]);
            if (u != nullptr) {
                userId = u->user_id;
                string username = u->user_name;
                cout << "<h2> Bienvenido " << username << " </h2>\n";
                isUserLogged = true;

                vector<Product> shoppingCartProducts = dbMgr->getProductsOnShoppingCart(userId);

                Order order = new Order(); // THIS IS THE REAL ORDER
                printOrderSummary(shoppingCartProducts, order);
            }
        }
    }

    return 0;
}