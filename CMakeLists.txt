cmake_minimum_required(VERSION 3.14)
project(DBacktest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src)
add_subdirectory(strategies)
add_subdirectory(tests)

add_executable(DBacktest main.cpp)
target_link_libraries(DBacktest PRIVATE src strategies)
