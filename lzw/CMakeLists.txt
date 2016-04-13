cmake_minimum_required(VERSION 3.5)
project(lzw)
add_executable(lzw main.cpp lzw.cpp)
set_property(TARGET lzw PROPERTY CXX_STANDARD 11)
