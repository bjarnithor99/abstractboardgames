#include "parser.hpp"
#include <chrono>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

const int N_MS = 10000;

int main(int argc, char *argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());

    Parser parser("/home/bjarni/HR/T-404-LOKA/cpp/games/tictactoe/tictactoe.game");
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
                    .count() > N_MS) {
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

    return 0;
}
