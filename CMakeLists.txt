cmake_minimum_required(VERSION 3.12)
project(MyCGI)

set(CMAKE_CXX_STANDARD 14)

add_executable(MyCGI add_product.cpp add_product_aux.cpp addcart.cpp send_message.cpp send_message_aux.cpp)
target_link_libraries(MyCGI mysqlcppconn)