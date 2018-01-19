# Make 最低版本号要求
cmake_minimum_required (VERSION 2.8)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
# 项目信息
project (press)
# valgrind
include (CTest)
# 指定include 路径
include_directories(.)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)
# 指定生成目标
add_executable(press ${DIR_SRCS})

