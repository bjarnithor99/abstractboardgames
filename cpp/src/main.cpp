#include "parser.hpp"
#include <chrono>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int cnt;

int minimax(Environment *env, int depth, bool max_player) {
    cnt++;
    if (env->variables.game_over || depth == 0)
        return env->variables.white_score;

    std::vector<std::vector<Step>> available_moves = env->generate_moves();

    if (available_moves.empty()) {
        return env->variables.white_score;
    }

    int value;
    if (max_player) {
        value = INT_MIN;
        for (const std::vector<Step> &move : available_moves) {
            env->execute_move(move);
            value = max(value, minimax(env, depth - 1, !max_player));
            env->undo_move();
        }
    }
    else {
        value = INT_MAX;
        for (const std::vector<Step> &move : available_moves) {
            env->execute_move(move);
            value = min(value, minimax(env, depth - 1, !max_player));
            env->undo_move();
        }
    }

    return value;
}

int main(int argc, char *argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());

    Parser parser("/home/bjarni/HR/T-404-LOKA/cpp/games/chess/chess.game");
    parser.parse();
    std::unique_ptr<Environment> env = parser.get_environment();

    std::string in;
    int move_count = 0;
    const int N_GAMES = 1;
    // auto start_time = std::chrono::system_clock::now();
    for (int game_number = 0; game_number < N_GAMES; game_number++) {
        // std::cout << "Game number: " << game_number << std::endl;
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
                cnt = 0;
                // auto s = std::chrono::system_clock::now();
                // env->execute_move(move);
                // minimax(env.get(), 7, env->get_current_player() == env->get_first_player());
                // std::cout << " with value " << minimax(env.get(), 5, env->current_player == "white");
                // env->undo_move();
                // auto e = std::chrono::system_clock::now();
                // std::cout << std::endl;
                // double running_time = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
                // std::cout << "Visited " << cnt << " states" << std::endl;
                // std::cout << "Running time (ms): " << running_time << std::endl;
                // std::cout << "States/s: " << cnt / running_time * 1000 << std::endl << std::endl;
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
                    // if (in == "r") {
                    // std::uniform_int_distribution<int> uni(0, found_moves.size() - 1);
                    // const std::vector<Step> &chosen_move = found_moves[uni(rng)];
                    // move_num = uni(rng);
                    //     }
                    //     else {
                    move_num = std::stoi(in);
                    //     }
                    const std::vector<Step> &chosen_move = found_moves[move_num];
                    //     std::cout << "Chosen move: ";
                    //     for (const Step &step : chosen_move) {
                    //         std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() <<
                    //         "} ";
                    //     }
                    //     std::cout << std::endl;
                    env->execute_move(chosen_move);
                    move_count++;
                }
            }
        }
    }
    // auto end_time = std::chrono::system_clock::now();
    // double running_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    // std::cout << "Running time (ms): " << running_time << std::endl;
    // std::cout << "Move count: " << move_count << std::endl;
    // std::cout << "Moves/s: " << move_count / running_time * 1000 << std::endl;
    // std::cout << "Games/s: " << N_GAMES / running_time * 1000 << std::endl;

    std::cout << "\n\n\n";
    env->print();
    std::cout << "GAME IS OVER!" << std::endl;
    std::cout << "Black score: " << env->variables.black_score << std::endl;
    std::cout << "White score: " << env->variables.white_score << std::endl;

    return 0;
}
