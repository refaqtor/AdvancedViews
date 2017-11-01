cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} --coverage")
set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_RELWITHDEBINFO} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} --coverage")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO} --coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} --coverage")

project(AdvancedViews)
add_subdirectory(AdvancedViews)
add_subdirectory(Example)
add_subdirectory(Test)
