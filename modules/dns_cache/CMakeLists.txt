project(dns_cache)

file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)

add_library(dns_cache ${SOURCES} ${HEADERS})
target_include_directories(dns_cache PUBLIC ${PROJECT_SOURCE_DIR})
