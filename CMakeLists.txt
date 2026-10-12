cmake_minimum_required(VERSION 3.14)
project(VirtualCarShowroom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

file(GLOB CG_SOURCES CONFIGURE_DEPENDS src/cg/*.cpp src/scene/*.cpp)
add_library(cg STATIC ${CG_SOURCES})
target_include_directories(cg PUBLIC include)

add_executable(showroom apps/showroom_main.cpp)
target_link_libraries(showroom PRIVATE cg)

add_executable(algorithms_demo apps/algorithms_demo.cpp)
target_link_libraries(algorithms_demo PRIVATE cg)

add_executable(test_algorithms tests/test_algorithms.cpp)
target_link_libraries(test_algorithms PRIVATE cg)

enable_testing()
add_test(NAME algorithms COMMAND test_algorithms)
