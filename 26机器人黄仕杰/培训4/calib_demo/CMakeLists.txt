cmake_minimum_required(VERSION 3.10)
project(calib_demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV: ${OpenCV_VERSION}")

add_executable(calib_demo main.cpp)
target_link_libraries(calib_demo ${OpenCV_LIBS})
