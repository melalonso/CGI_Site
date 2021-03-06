#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <regex>
#include <cstdlib>
#include <cppconn/prepared_statement.h>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

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
        stmt = con->createStatement();
        res = stmt->executeQuery("select * from shop_cart sc join products pr on sc.product_id=pr.product_id "
                                 "where sc.user_id=" + to_string(user_id) + ";");
        while (res->next()) {
            int id = res->getInt(5);
            string name = res->getString(6);
            string description = res->getString(7);
            int price = res->getInt(8);
            int quantity = res->getInt(3);
            ShopCartProduct p(id, name, description, price, quantity);
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


    long insertOrder(Order o) {
        pstmt = con->prepareStatement(
                "INSERT INTO orders(user_id, shipping_address, city, state, country) VALUES (?,?,?,?,?)");
        pstmt->setInt(1, o.userId);
        pstmt->setString(2, o.shippingAddress);
        pstmt->setString(3, o.city);
        pstmt->setString(4, o.state);
        pstmt->setString(5, o.country);
        pstmt->execute();
        stmt = con->createStatement();
        sql::ResultSet *res = stmt->executeQuery("SELECT @@identity AS id;");
        res->next();
        return res->getInt64("id");

    }

    void insertOrderProduct(int orderId, int productId, int quantity) {
        pstmt = con->prepareStatement(
                "INSERT INTO order_products(order_id, product_id, quantity) VALUES (?,?,?)");
        pstmt->setInt(1, orderId);
        pstmt->setInt(2, productId);
        pstmt->setInt(3, quantity);
        pstmt->execute();
    }

    void cleanShopCart(int userId) {
        stmt = con->createStatement();
        stmt->execute("delete from shop_cart where user_id=" + to_string(userId));
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


void printOrderSummary(vector<ShopCartProduct> products, Order order) {
    int total = 0;
    int subTotal = 0;
    cout << "<h2>Thank you!</h2>\n";
    cout << "<p>Shipping address: " << order.shippingAddress << "</p>";
    cout << "<p>City: " << order.city << "</p>";
    cout << "<p>State: " << order.state << "</p>";
    cout << "<p>Country: " << order.country << "</p>";

    cout << "<h4>Products: </h4>\n";
    cout << "<ul>\n";
    for (auto product : products) {
        cout << "<li>" << product.quantity << " " << product.name << " for $" << product.price << " each.</li>\n";
        subTotal += product.price * product.quantity;
    }
    cout << "</ol>\n";
    total = subTotal + (subTotal * 0.10);
    cout << "<b>Subtotal: " << subTotal << "<br>\n";
    cout << "<b>Total to pay (subtotal + shipping): " << total << "<br>\n";
    cout << "<a href='/cgi-bin/index'>Back</a>";
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


/* returns Str with all characters with special HTML meanings converted to
  entity references. */
string escapeHTML(string &Str) {
    string escaped = "";
    for (int i = 0; i < Str.size(); ++i) {
        string ThisCh = Str.substr(i, 1);
        if (ThisCh == "<")
            ThisCh = "&lt;";
        else if (ThisCh == ">")
            ThisCh = "&gt;";
        else if (ThisCh == "\"")
            ThisCh = "&quot;";
        else if (ThisCh == "'")
            ThisCh = "&apos;";
        else if (ThisCh == "&")
            ThisCh = "&amp;";
        escaped += ThisCh;
    }
    return escaped;
}


// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
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

                    vector<ShopCartProduct> shoppingCartProducts = dbMgr->getProductsOnShoppingCart(userId);

                    string input;
                    cin >> input;
                    map<string, string> form_parameters;
                    form_parameters = parse(input);

                    string shippingAddress = decode(form_parameters["shipping_address"]);
                    string city = decode(form_parameters["city"]);
                    string state = decode(form_parameters["state"]);
                    string country = decode(form_parameters["country"]);
                    shippingAddress = escapeHTML(shippingAddress);
                    city = escapeHTML(city);
                    state = escapeHTML(state);
                    country = escapeHTML(country);


                    if (shippingAddress.size() > 100 || city.size() > 50 || state.size() > 50 || country.size() > 50) {
                        cout << "Max lengths are 100 for shipping address and 50 for city, state and country<br>";
                        cout << "<a href='/cgi-bin/index'>Volver</a>";
                        return 0;
                    } else if (trim(shippingAddress) == "" || trim(city) == "" || trim(state) == "" ||
                               trim(country) == "") {
                        cout << "Data could not be empty<br>";
                        cout << "<a href='/cgi-bin/index'>Volver</a>";
                        return 0;
                    }

                    Order order(-1, userId, shippingAddress, city, state, country);


                    long orderId = dbMgr->insertOrder(order);
                    for (ShopCartProduct p : shoppingCartProducts) {
                        int productId = p.product_id;
                        int quantity = p.quantity;
                        dbMgr->insertOrderProduct(orderId, productId, quantity);
                    }
                    dbMgr->cleanShopCart(userId);
                    printOrderSummary(shoppingCartProducts, order);

                }
            }
        }
    } else {
        cout << "Log back in<br>";
        cout << "<a href='/cgi-bin/index'>Volver</a>";
    }

    return 0;
}