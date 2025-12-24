cmake_minimum_required(VERSION 3.5...4.0)

project(Example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(${PROJECT_NAME} "src/main.cpp")
add_subdirectory(.. "${CMAKE_CURRENT_BINARY_DIR}/argless")
target_link_libraries(${PROJECT_NAME} PRIVATE argless)
