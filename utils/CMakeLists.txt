project(utils)
cmake_minimum_required(VERSION 3.17)
project (utils)

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE ${PROJECT_SOURCE_DIR})