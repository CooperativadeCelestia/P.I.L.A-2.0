cmake_minimum_required(VERSION 3.20)

project(PILA)

add_executable(pila-test1 src/common.c src/sha256.c src/string_helper.c src/Test1.c)
add_executable(pila-test2 src/wallet.c src/string_helper.c src/Test2.c)

target_include_directories(pila-test1 PRIVATE include)
target_include_directories(pila-test2 PRIVATE include)