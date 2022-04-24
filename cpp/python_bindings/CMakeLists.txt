cmake_minimum_required(VERSION 3.12)

project(python_bindings)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")

file(GLOB_RECURSE abg_SOURCES CONFIGURE_DEDPENDS "../src/*.cpp")
file(GLOB_RECURSE abg_HEADERS CONFIGURE_DEDPENDS "../src/*.hpp")

set(abg_INCLUDE_DIRS "")
foreach(_header_file ${abg_HEADERS})
  get_filename_component(_dir ${_header_file} PATH)
  list(APPEND abg_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES abg_INCLUDE_DIRS)

add_library(abg_lib SHARED ${abg_SOURCES})

include_directories(abg_lib PRIVATE ${abg_INCLUDE_DIRS})

find_package(pybind11 REQUIRED)

pybind11_add_module(python_bindings python_bindings.cpp)
target_link_libraries(python_bindings PRIVATE abg_lib)
