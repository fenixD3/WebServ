cmake_minimum_required(VERSION 3.21)

project(webserv)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/srcs/*.cpp ${PROJECT_SOURCE_DIR}/srcs/Logger/*.cpp)
message(STATUS "${SOURCE}")

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE includes)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND mv ${PROJECT_BINARY_DIR}/${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
