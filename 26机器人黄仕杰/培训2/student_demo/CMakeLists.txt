cmake_minimum_required(VERSION 3.11)

project(CLASS_2)

include_directories(includes)

### 1 ###

add_executable(main_1 ./src/main.cpp ./tools/student.cpp)

### 2 ###

add_library(TOOLS_STATIC STATIC ./tools/student.cpp)
add_executable(cmake_learn_1 src/main.cpp)
target_link_libraries(cmake_learn_1 TOOLS_STATIC)