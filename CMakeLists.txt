cmake_minimum_required(VERSION 3.29.2)
project(Ciri CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

message("ASAN: " ${ENABLE_ASAN})
message("MSAN: " ${ENABLE_MSAN})
message("TSAB: " ${ENABLE_TSAN})
message("UBSAN: " ${ENABLE_UBSAN})
if(ENABLE_ASAN)
    message(STATUS "Enabling AddressSanitizer")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
elseif(ENABLE_MSAN)
    message(STATUS "Enabling MemorySanitizer")
    add_compile_options(-fsanitize=memory -fno-omit-frame-pointer)
    add_link_options(-fsanitize=memory)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif(ENABLE_TSAN)
    message(STATUS "Enabling ThreadSanitizer")
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread)
elseif(ENABLE_UBSAN)
    message(STATUS "Enabling UndefinedBehaviorSanitizer")
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
else()
    add_compile_options(-O3)
endif()

set(SOURCES
    src/metrics/counter_basic.cpp
)
set(HEADERS
    src/metrics/counter_sum.h
    src/metrics/counter_minmax.h
    src/metrics/counter_basic.h
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
