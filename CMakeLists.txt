#设置最低版本和项目名称
cmake_minimum_required(VERSION 3.0)

project(Linux_TCP)

#设置全局的C++标准
set(CMAKE_CXX_STANDARD 11)#在这里我设置了11，代表使用c++11标准
set(CMAKE_CXX_STANDARD_REQUIRED True)#CMAKE_CXX_STANDARD_REQUIRED表示强制使用指定的标准


#查找porotbuf包
#find_package(Protobuf REQUIRED)
#include_directories(${Protobuf_INCLUDE_DIRS})#Protobuf_INCLUDE_DIRS表示protobuf头文件目录


add_subdirectory(src)