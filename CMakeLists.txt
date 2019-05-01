cmake_minimum_required(VERSION 3.13)
project(database4_1)

set(CMAKE_CXX_STANDARD 14)

add_executable(database4_1 main.cpp extrem.cpp extrem.h extrem.h select.h CNode.cpp CNode.h BPlus_tree.h join.h project.h sort.h)