#include "parser.hpp"
#include <cassert>
#include <chrono>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

long long state_cnt = 1;
const int DEPTH = 20;

void search(Environment *env, int depth) {
    state_cnt++;

    if (env->variables.game_over || depth == 0)
        return;

    std::vector<std::vector<Step>> available_moves = env->generate_moves();

    for (const std::vector<Step> &move : available_moves) {
        env->execute_move(move);
        search(env, depth - 1);
        env->undo_move();
    }
}

int main(int argc, char *argv[]) {
    assert(DEPTH > 0);
    std::random_device rd;
    std::mt19937 rng(rd());

    Parser parser("/home/bjarni/HR/T-404-LOKA/cpp/games/tictactoe/tictactoe.game");
    parser.parse();
    std::unique_ptr<Environment> env = parser.get_environment();

    auto start_time = std::chrono::system_clock::now();

    std::vector<std::vector<Step>> found_moves = env->generate_moves();
    for (const std::vector<Step> &move : found_moves) {
        env->execute_move(move);
        search(env.get(), DEPTH - 1);
        env->undo_move();
    }

    if (!found_moves.empty()) {
        std::uniform_int_distribution<int> uni(0, found_moves.size() - 1);
        const std::vector<Step> &chosen_move = found_moves[uni(rng)];
        env->execute_move(chosen_move);
    }

    auto end_time = std::chrono::system_clock::now();
    double running_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "Running time (ms): " << running_time << std::endl;
    std::cout << "Number of states visited: " << state_cnt << std::endl;
    std::cout << std::fixed << "States/s: " << state_cnt / running_time * 1000 << std::endl;

    return 0;
}
