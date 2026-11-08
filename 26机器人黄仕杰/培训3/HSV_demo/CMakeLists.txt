cmake_minimum_required(VERSION 3.11)

project(practice)

find_package(OpenCV REQUIRED)
#生成说明书compile_commands.json
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 告诉 CMake 编译 ，并生成叫main的程序
add_executable(main main.cpp)

# 给 class-2 链接 OpenCV 库
target_link_libraries(main PRIVATE ${OpenCV_LIBS})

#为目标指定包含头文件的目录||去这里面找头文件
target_include_directories(main PRIVATE ${OpenCV_INCLUDE_DIRS})

