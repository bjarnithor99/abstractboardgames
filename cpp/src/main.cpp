// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file main.cpp
 *  @brief A simple TUI to play games described using Abstract Boardgames.
 *  @author Bjarni Dagur Thor Kárason
 */
#include "parser.hpp"
#include <chrono>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

/// @brief Takes an Abstract Boardgame description and runs it an a TUI.
/// @author Bjarni Dagur Thor Kárason
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <gamefile>" << std::endl;
        return EXIT_FAILURE;
    }

    std::random_device rd;
    std::mt19937 rng(rd());

    Parser parser(argv[1]);
    parser.parse();
    std::unique_ptr<Environment> env = parser.get_environment();

    std::string in;
    int move_count = 0;
    env->reset();
    while (!env->variables.game_over) {
        if (move_count != 0)
            std::cout << "\n\n\n";
        std::cout << env->current_player << " turn:" << std::endl;
        env->print();

        std::vector<std::vector<Step>> found_moves = env->generate_moves();
        int i = 0;
        for (const std::vector<Step> &move : found_moves) {
            std::cout << i++ << ") ";
            for (const Step &step : move) {
                std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() << "} ";
            }
            std::cout << std::endl;
        }

        if (!found_moves.empty()) {
            std::cout << "r) Random move" << std::endl;
            std::cout << "u) Undo" << std::endl;
            std::cout << "q) Quit" << std::endl;

            std::cin >> in;
            if (in == "q") {
                return 0;
            }
            else if (in == "u") {
                env->undo_move();
            }
            else {
                int move_num;
                if (in == "r") {
                    std::uniform_int_distribution<int> uni(0, found_moves.size() - 1);
                    move_num = uni(rng);
                }
                else {
                    move_num = std::stoi(in);
                }
                const std::vector<Step> &chosen_move = found_moves[move_num];
                std::cout << "Chosen move: ";
                for (const Step &step : chosen_move) {
                    std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() << "}";
                }
                std::cout << std::endl;
                env->execute_move(chosen_move);
                move_count++;
            }
        }
    }

    std::cout << "\n\n\n";
    env->print();
    std::cout << "GAME IS OVER!" << std::endl;
    std::cout << "Black score: " << env->variables.black_score << std::endl;
    std::cout << "White score: " << env->variables.white_score << std::endl;

    return EXIT_SUCCESS;
}
