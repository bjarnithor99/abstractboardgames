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

    std::ifstream input_file("games/chess_small.game");
    Parser parser(&input_file);
    parser.parse();
    std::unique_ptr<Environment> env = parser.get_environment();

    std::string in;
    std::vector<std::string> players = {"white", "black"};
    int move_count = 0;
    bool game_over = false;
    while (!game_over) {
        if (move_count != 0)
            std::cout << "\n\n\n";
        std::cout << players[move_count % players.size()] << " turn:" << std::endl;
        env->print();
        std::cin >> in;
        if (in == "quit")
            return 0;
        env->generate_moves(players[move_count % players.size()]);
        for (const std::vector<Step> &move : env->found_moves) {
            for (const Step &step : move) {
                std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() << "} ";
            }
            std::cout << std::endl;
        }
        if (!env->found_moves.empty()) {
            std::uniform_int_distribution<int> uni(0, env->found_moves.size() - 1);
            const std::vector<Step> &chosen_move = env->found_moves[uni(rng)];
            std::cout << "Chose move at random: ";
            for (const Step &step : chosen_move) {
                std::cout << "(" << step.x << ", " << step.y << "){" << step.side_effect->get_name() << "} ";
            }
            game_over = env->execute_move(chosen_move);
            move_count++;
        }
        else {
            game_over = env->check_terminal_conditions();
        }
    }

    std::cout << "GAME IS OVER!" << std::endl;

    input_file.close();

    return 0;
}
