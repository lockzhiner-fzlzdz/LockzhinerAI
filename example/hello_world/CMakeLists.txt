# CMake最低版本要求  
cmake_minimum_required(VERSION 3.10)  
  
# 设置项目名称和版本  
project(HelloWorld VERSION 1.0)  
  
# 指定C++标准  
set(CMAKE_CXX_STANDARD 11) # 设置C++11为标准  
set(CMAKE_CXX_STANDARD_REQUIRED True) # 要求必须支持C++11标准  
  
# 添加可执行文件  
# hello.cpp是源文件，HelloWorld是生成的可执行文件名  
add_executable(HelloWorld hello_world.cpp)