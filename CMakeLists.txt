cmake_minimum_required(VERSION 2.8.12)
project(wordle CXX)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(wordle ${SRC_FILES})
target_link_libraries(wordle ${CONAN_LIBS})
