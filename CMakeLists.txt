cmake_minimum_required(VERSION 3.31.4)
project(Ciri)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/metrics/counter.cpp
)
set(HEADERS
    src/metrics/counter.h
)
add_library(ciri STATIC ${SOURCES} ${HEADERS})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

target_include_directories(ciri PUBLIC src)
add_subdirectory(tests)