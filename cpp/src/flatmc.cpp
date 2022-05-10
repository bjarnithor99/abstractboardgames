// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file flatmc.cpp
 *  @brief A benchmarking tool using the flat Monte-Carlo measure.
 *  @author Bjarni Dagur Thor Kárason
 */
#include "parser.hpp"
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

/// @brief Takes an Abstract Boardgame description and timelimit in
///  milliseconds, and runs flat Monte-Carlo rollouts until the time runs out.
///  Prints relevant statistics.
/// @author Bjarni Dagur Thor Kárason
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <gamefile> <timelimit>" << std::endl;
        return EXIT_FAILURE;
    }

    int n_ms = std::stoi(argv[2]);
    assert(n_ms > 0);

    std::random_device rd;
    std::mt19937 rng(rd());

    Parser parser(argv[1]);
    parser.parse();
    std::unique_ptr<Environment> env = parser.get_environment();

    int game_count = 0;
    long long int state_count = 0;
    bool time_left = true;
    auto start_time = std::chrono::system_clock::now();
    while (time_left) {
        game_count++;
        env->reset();
        while (!env->variables.game_over) {
            state_count++;

            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time)
                    .count() > n_ms) {
                time_left = false;
                break;
            }

            std::vector<std::vector<Step>> found_moves = env->generate_moves();

            if (!found_moves.empty()) {
                std::uniform_int_distribution<int> uni(0, found_moves.size() - 1);
                const std::vector<Step> &chosen_move = found_moves[uni(rng)];
                env->execute_move(chosen_move);
            }
        }
    }
    auto end_time = std::chrono::system_clock::now();
    double running_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Running time (ms): " << running_time << std::endl;
    std::cout << "Games played: " << game_count << std::endl;
    std::cout << "Games/s: " << game_count / running_time * 1000 << std::endl;
    std::cout << "States visited: " << state_count << std::endl;
    std::cout << "States/s: " << state_count / running_time * 1000 << std::endl;

    return EXIT_SUCCESS;
}
