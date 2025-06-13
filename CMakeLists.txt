cmake_minimum_required(VERSION 3.29.2)
project(ciri LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

message("ASAN: " ${ENABLE_ASAN})
message("MSAN: " ${ENABLE_MSAN})
message("TSAN: " ${ENABLE_TSAN})
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

set(CIRI_SOURCES
    include/ciri/metrics/CounterBasic.h
    include/ciri/metrics/CounterSum.h
    include/ciri/metrics/CounterMinMax.h
    include/ciri/utils/LinearHash.h src/utils/LinearHash.cpp
    include/ciri/streaming/CountMinSketch.h
)
add_library(${PROJECT_NAME} ${CIRI_SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE atomic)
install(TARGETS ciri)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
