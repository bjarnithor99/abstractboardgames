#include "parser.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());

    std::ifstream input_file("games/breakthrough_small.game");
    Parser parser(&input_file);
    parser.parse();
    std::unique_ptr<Environment> env = parser.get_environment();

    std::string in;
    int move_count = 0;
    bool game_over = false;
    while (!game_over) {
        if (move_count != 0)
            std::cout << "\n\n\n";
        std::cout << env->current_player << " turn:" << std::endl;
        env->print();

        env->generate_moves();
        int i = 0;
        for (const std::vector<Step> &move : env->found_moves) {
            std::cout << i++ << ") ";
            for (const Step &step : move) {
                std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() << "} ";
            }
            std::cout << std::endl;
        }

        if (!env->found_moves.empty()) {
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
                    std::uniform_int_distribution<int> uni(0, env->found_moves.size() - 1);
                    move_num = uni(rng);
                }
                else {
                    move_num = std::stoi(in);
                }
                const std::vector<Step> &chosen_move = env->found_moves[move_num];
                std::cout << "Chosen move: ";
                for (const Step &step : chosen_move) {
                    std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() << "} ";
                }
                std::cout << std::endl;
                game_over = env->execute_move(chosen_move);
                move_count++;
            }
        }
        else {
            game_over = env->check_terminal_conditions();
        }
    }

    std::cout << "\n\n\n";
    env->print();
    std::cout << "GAME IS OVER!" << std::endl;
    std::cout << "Black score: " << env->variables.blackScore << std::endl;
    std::cout << "White score: " << env->variables.whiteScore << std::endl;

    input_file.close();

    return 0;
}
