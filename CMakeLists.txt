cmake_minimum_required(VERSION 3.13)
project(mirrorClient)

set(CMAKE_CXX_STANDARD 98)

add_executable(mirrorClient main.cpp Arguments.cpp Arguments.hpp Program.cpp Program.hpp FileUtils.cpp FileUtils.hpp List.hpp ListNode.hpp Logger.cpp Logger.hpp)