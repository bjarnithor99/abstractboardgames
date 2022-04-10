cmake_minimum_required(VERSION 3.12)

project(python-interface)

add_library(abg_lib SHARED
    ../src/parsing/ast.cpp
    ../src/state_machines/dfa.cpp
    ../src/environment/environment.cpp
    ../src/state_machines/fa_tools.cpp
    ../src/parsing/lexer.cpp
    ../src/main.cpp
    ../src/state_machines/nfa.cpp
    ../src/visitors/nfa_visitor.cpp
    ../src/parsing/parser.cpp
    ../src/actions/predicates.cpp
    ../src/visitors/print_visitor.cpp
    ../src/actions/side_effects.cpp
    ../src/actions/terminal_conditions.cpp
    ../src/actions/variables.cpp
)

include_directories(abg_lib PRIVATE
    ../src/actions
    ../src/environment
    ../src/parsing
    ../src/state_machines
    ../src/visitors
)


find_package(pybind11 REQUIRED)

pybind11_add_module(py_interface py_interface.cpp)
target_link_libraries(py_interface PRIVATE abg_lib)
