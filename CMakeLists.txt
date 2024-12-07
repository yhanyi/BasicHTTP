cmake_minimum_required(VERSION 3.28)
project(BasicHTTP)

set(CMAKE_CXX_STANDARD 17)

add_executable(BasicHTTP
  main.cpp
  server.cpp
  server.hpp
)
