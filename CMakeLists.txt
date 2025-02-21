cmake_minimum_required(VERSION 3.19)
project(yazi-web)
#设置c++标准为11
set(CMAKE_CXX_STANDARD 11)      
#设置utf-8编译选项
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
#把当前目录添加到包含目录列表
include_directories(./)
#递归搜索(glob_recurse)所有c++源文件,如下都是一堆目录/*.cpp,然后注入sources变量
#cmakelist所在位置,是和下面这些目录是同级的,main函数也是同级的
file(GLOB_RECURSE SOURCES "app/*.cpp" "frame/*.cpp" "fs/*.cpp" "json/*.cpp" "reflect/*.cpp" "socket/*.cpp" "task/*.cpp" "thread/*.cpp" "utility/*.cpp" "web/*.cpp")
#创建一个main的可执行文件
add_executable(main ${SOURCES} main.cpp)
