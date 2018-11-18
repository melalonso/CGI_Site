#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <regex>
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

    vector<Product> searchProducts(string keyword) {
        vector<Product> results;
        pstmt = con->prepareStatement("SELECT * from products where name like ? or description like ?;");
        pstmt->setString(1, keyword);
        pstmt->setString(2, keyword);
        res = pstmt->executeQuery();
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

    void insertOrderProduct(int orderId, int productId) {
        pstmt = con->prepareStatement(
                "INSERT INTO order_products(order_id, user_id) VALUES (?,?)");
        pstmt->setInt(1, orderId);
        pstmt->setInt(2, productId);
        pstmt->execute();
    }

    void insertMessage(string name, string text) {
        pstmt = con->prepareStatement(
                "INSERT INTO messages(person_name, message) VALUES (?,?)");
        pstmt->setString(1, name);
        pstmt->setString(2, text);
        pstmt->execute();
    }


};

void print_products(vector<Product> products, bool isUserLogged) {
    for (auto product : products) {
        cout << "\t\t<div class='product' style='float:left;'>\n";
        cout << "\t\t\t<p>Code: " << product.product_id << "</p>\n";
        cout << "\t\t\t<p>Name: " << product.name << "</p>\n";
        cout << "\t\t\t<p>Description: " << product.description << "</p>\n";
        cout << "\t\t\t<p>Price: " << product.price << "</p>\n";
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

    string input;
    cin >> input;
    map<string, string> parameters = parse(input);

    string name = decode(parameters["name"]);
    string message = decode(parameters["message"]);
    name = escapeHTML(name);
    message = escapeHTML(message);

    if (name.size() > 50) name = name.substr(0, 50);
    if (message.size() > 300) message = message.substr(300);

    if (trim(name) == "" || trim(message) == "") {
        cout << "Values could not be empty nor be spaces<br>";
        cout << "<a href='/cgi-bin/index'>Volver</a>";
        return 0;
    }

    dbMgr->insertMessage(name, message);
    cout << "Message was sent sucessfully<br>";
    cout << "<a href='/cgi-bin/index'>Volver</a>";

    delete dbMgr;

    return 0;
}