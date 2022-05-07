cmake_minimum_required(VERSION 3.12)

project(abstract-board-games)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Ofast -march=native")

add_subdirectory(python_bindings)

file(GLOB_RECURSE abg_SOURCES CONFIGURE_DEDPENDS "src/*.cpp")
file(GLOB_RECURSE abg_HEADERS CONFIGURE_DEDPENDS "src/*.hpp")

list(FILTER abg_SOURCES EXCLUDE REGEX ".*/src/main.cpp")
list(FILTER abg_SOURCES EXCLUDE REGEX ".*/src/perft.cpp") 
list(FILTER abg_SOURCES EXCLUDE REGEX ".*/src/flatmc.cpp")
get_filename_component(main_file src/main.cpp ABSOLUTE)
get_filename_component(perft_file src/perft.cpp ABSOLUTE)
get_filename_component(flatmc_file src/flatmc.cpp ABSOLUTE)

set(abg_INCLUDE_DIRS "")
foreach(_header_file ${abg_HEADERS})
  get_filename_component(_dir ${_header_file} PATH)
  list(APPEND abg_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES abg_INCLUDE_DIRS)

add_executable(abstract-board-games ${main_file} ${abg_SOURCES})
add_executable(perft-abstract-board-games ${perft_file} ${abg_SOURCES})
add_executable(flatmc-abstract-board-games ${flatmc_file} ${abg_SOURCES})
target_include_directories(abstract-board-games PRIVATE ${abg_INCLUDE_DIRS})
target_include_directories(perft-abstract-board-games PRIVATE ${abg_INCLUDE_DIRS})
target_include_directories(flatmc-abstract-board-games PRIVATE ${abg_INCLUDE_DIRS})

option(BUILD_DOCS "Build documentation" OFF)
if (BUILD_DOCS)
    find_package(Doxygen
                 REQUIRED dot)

    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen executable not found")
    endif()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(docs
                      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating Doxygen documentation" VERBATIM)
endif()
