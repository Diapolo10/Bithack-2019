cmake_minimum_required(VERSION 3.10)

project(bithack_base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(asio INTERFACE)
target_include_directories(asio INTERFACE asio-1.13.0/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE=1)

if (WIN32)
    target_link_libraries(asio INTERFACE Ws2_32)
endif()


add_executable(bithack_base
    src/main.cpp
    src/motor_control/link.cpp
    src/motor_control/reporter.cpp)

target_link_libraries(bithack_base PRIVATE asio)
