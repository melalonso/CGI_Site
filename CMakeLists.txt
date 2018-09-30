cmake_minimum_required(VERSION 3.12)
project(MyCGI)

set(CMAKE_CXX_STANDARD 14)

add_executable(MyCGI main.cpp add_product.cpp add_product.h DatabaseManager.cpp DatabaseManager.h add_product_aux.cpp add_product_aux.h addcart.cpp addcart.h)
target_link_libraries(MyCGI mysqlcppconn)