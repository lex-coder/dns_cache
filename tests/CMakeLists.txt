project(tests)
cmake_minimum_required(VERSION 3.17)

enable_testing()

add_executable(${PROJECT_NAME} dns_cache.cpp)
target_link_libraries(${PROJECT_NAME} dns_cache Catch2 Catch2WithMain)
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests
)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

