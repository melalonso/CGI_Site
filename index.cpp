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


};

void print_products(vector<Product> products, bool isUserLogged) {
    for (auto product : products) {
        cout << "\t\t<div class='product' style='float:left;'>\n";
        cout << "\t\t\t<p>Codigo: " << product.product_id << "</p>\n";
        cout << "\t\t\t<p>Nombre: " << product.name << "</p>\n";
        cout << "\t\t\t<p>Descripcion: " << product.description << "</p>\n";
        cout << "\t\t\t<p>Precio: " << product.price << "</p>\n";
        if (isUserLogged) {
            cout << "<a href=\"/cgi-bin/addcart?pid=" << product.product_id << "\">Add to Cart</a><br><br>\n";
        }
        cout << "\t\t</div>\n";
    }
}


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


int main() {
    cout << "Content-type:text/html\r\n\r\n";
    DatabaseManager *dbMgr = new DatabaseManager();
    bool isUserLogged = false;
    string ssid = "";

    if (const char *env_p = std::getenv("HTTP_COOKIE")) {
        ssid = string(env_p);
        map<string, string> parameters = parse(ssid);
        if (!parameters.empty() && parameters["sid"] != "") {
            User *u = dbMgr->getUserWithCookie(parameters["sid"]);
            if (u != nullptr) {
                string username = u->user_name;
                cout << "<h2> Bienvenido " << username << " </h2>\n";
                isUserLogged = true;
            }
        }
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
            "\t\t</div>\n";

    if (!isUserLogged) {
        cout << "\t\t<div id='login'>\n"
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
    } else {
        cout << "\t\t<div id='logout'>\n"
                "\t\t\t<h3><a href='/cgi-bin/logout'>Logout</a></h3>\n"
                "\t\t</div>";
        cout << "\t\t<div id='add_product'>\n"
                "\t\t\t<h3><a href='/cgi-bin/add_product'>Sell product</a></h3>\n"
                "\t\t</div>";
    }

    cout << "\t<div id='content'>\n";

    vector<Product> products = dbMgr->getProducts();
    print_products(products, isUserLogged);
    cout << "\t</div>\n"
            "</body>\n"
            "</html>";
    delete dbMgr;

    return 0;
}