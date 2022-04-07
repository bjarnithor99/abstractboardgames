cmake_minimum_required(VERSION 3.12)

project(abstract-board-games)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")

file(GLOB_RECURSE abg_SOURCES CONFIGURE_DEDPENDS "src/*.cpp")
file(GLOB_RECURSE abg_HEADERS CONFIGURE_DEDPENDS "src/*.hpp")

set(abg_INCLUDE_DIRS "")
foreach(_header_file ${abg_HEADERS})
  get_filename_component(_dir ${_header_file} PATH)
  list(APPEND abg_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES abg_INCLUDE_DIRS)

add_executable(abstract-board-games ${abg_SOURCES})
target_include_directories(abstract-board-games PRIVATE ${abg_INCLUDE_DIRS})
