cmake_minimum_required(VERSION 3.10)

# 项目名称
project(HLS_Project)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(src)

# 查找所有源文件
file(GLOB SOURCES "src/*.cpp")

# 添加可执行文件
add_executable(hls ${SOURCES})

# 启用调试信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG -Wall")

# 链接库（如果有需要）
# target_link_libraries(hls <library_name>)