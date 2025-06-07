cmake_minimum_required(VERSION 3.29.2)
project(Ciri)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

message("ASAN: " ${ENABLE_ASAN})
message("MSAN: " ${ENABLE_MSAN})
message("UBSAN: " ${ENABLE_UBSAN})

if(ENABLE_ASAN OR ENABLE_MSAN OR ENABLE_UBSAN)
    set(SANITIZER_FLAGS "")
    set(SANITIZER_LINK_FLAGS "")
    
    if(ENABLE_ASAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=address")
        list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=address")
        add_compile_options(-fno-omit-frame-pointer)
    endif()
    
    if(ENABLE_MSAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=memory")
        list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=memory")
        add_compile_options(-fno-omit-frame-pointer)
        # MSan requires instrumented libc++
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
    
    if(ENABLE_UBSAN)
        list(APPEND SANITIZER_FLAGS "-fsanitize=undefined")
        list(APPEND SANITIZER_LINK_FLAGS "-fsanitize=undefined")
        # Optional: make UBSAN recoverable
        # list(APPEND SANITIZER_FLAGS "-fno-sanitize-recover=all")
    endif()
    
    string(REPLACE ";" " " SANITIZER_FLAGS_STR "${SANITIZER_FLAGS}")
    string(REPLACE ";" " " SANITIZER_LINK_FLAGS_STR "${SANITIZER_LINK_FLAGS}")
    
    add_compile_options(${SANITIZER_FLAGS})
    add_link_options(${SANITIZER_LINK_FLAGS})
endif()
add_compile_options(-O3)

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