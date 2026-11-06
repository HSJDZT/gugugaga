cmake_minimum_required(VERSION 3.10)
project(opencv_demo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 寻找 OpenCV 库，务必指明版本号 4
find_package(OpenCV 4 REQUIRED)

# 打印一下找到的 OpenCV 信息，方便调试
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# 添加头文件搜索路径（这一行很关键）
include_directories(${OpenCV_INCLUDE_DIRS})

# 生成可执行文件，你的源文件是 main.cpp
add_executable(${PROJECT_NAME} main.cpp)

# 链接 OpenCV 库
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})