// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "environment.hpp"
#include "parser.hpp"
#include "side_effects.hpp"
#include "variables.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(py_interface, m) {
    py::class_<Parser>(m, "Parser")
        .def(py::init<std::string>())
        .def("parse", &Parser::parse)
        .def("get_environment", &Parser::get_environment);

    py::class_<Environment>(m, "Environment")
        .def_readonly("board_size_x", &Environment::board_size_x)
        .def_readonly("board_size_y", &Environment::board_size_y)
        .def_readonly("current_player", &Environment::current_player)
        .def_readonly("variables", &Environment::variables)
        .def("get_environment_representation", &Environment::get_environment_representation)
        .def("generate_moves", py::overload_cast<>(&Environment::generate_moves), py::return_value_policy::move)
        .def("execute_move", &Environment::execute_move)
        .def("undo_move", &Environment::undo_move)
        .def("check_terminal_conditions", &Environment::check_terminal_conditions)
        .def("reset", &Environment::reset)
        .def("print", &Environment::print);

    py::class_<Variables>(m, "Variables")
        .def_readonly("black_score", &Variables::black_score)
        .def_readonly("white_score", &Variables::white_score)
        .def_readonly("game_over", &Variables::game_over);

    py::class_<Cell>(m, "Cell")
        .def_readonly("piece", &Cell::piece)
        .def_readonly("player", &Cell::player);

    py::class_<Step>(m, "Step")
        .def_readonly("x", &Step::x)
        .def_readonly("y", &Step::y)
        .def_readonly("side_effect", &Step::side_effect);

    py::class_<SideEffect, std::shared_ptr<SideEffect>>(m, "SideEffect").def("get_name", &SideEffect::get_name);
}
