// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
#include "fa_tools.hpp"

std::set<DFAState *> FATools::get_all_states(DFAState *initial_state) {
    std::set<DFAState *> vis;
    std::queue<DFAState *> q;
    q.push(initial_state);
    vis.insert(initial_state);
    while (!q.empty()) {
        DFAState *at = q.front();
        q.pop();
        for (const auto &p : at->transition) {
            DFAState *next_state = p.second;
            if (vis.find(next_state) == vis.end()) {
                q.push(next_state);
                vis.insert(next_state);
            }
        }
    }
    return vis;
}

std::set<NFAState *> FATools::get_all_states(NFAState *initial_state) {
    std::set<NFAState *> vis;
    std::queue<NFAState *> q;
    q.push(initial_state);
    vis.insert(initial_state);
    while (!q.empty()) {
        NFAState *at = q.front();
        q.pop();
        vis.insert(at);
        for (const auto &p : at->transitions) {
            const std::vector<NFAState *> &next_states = p.second;
            for (NFAState *next_state : next_states) {
                if (vis.find(next_state) == vis.end()) {
                    q.push(next_state);
                    vis.insert(next_state);
                }
            }
        }
    }
    return vis;
}

void FATools::to_dot(DFAState *initial_state, std::string output_path) {
    std::ofstream output_file;
    output_file.open(output_path);
    if (!output_file.is_open()) {
        std::string error_msg = "Failed to open file " + output_path;
        throw std::runtime_error(error_msg);
    }

    int next_node_number = 0;
    std::map<DFAState *, std::string> state2name;
    std::set<DFAState *> all_states = get_all_states(initial_state);

    output_file << "digraph graphname {\n";
    for (DFAState *state : all_states) {
        state2name[state] = "Node " + std::to_string(next_node_number++);
        output_file << "\t\"" << state2name[state] << "\"";
        if (state->is_accepting) {
            output_file << " [style=filled, fillcolor=green]";
        }
        output_file << "\n";
    }

    for (DFAState *state : all_states) {
        for (const auto &p : state->transition) {
            const DFAInput &input = p.first;
            DFAState *next_state = p.second;
            std::string label = "(" + std::to_string(input.dx) + ", " + std::to_string(input.dy) + ", " +
                                input.predicate->get_name() + ") {" + input.side_effect->get_name() + "}";
            output_file << "\t\"" << state2name[state] << "\" -> \"" << state2name[next_state] << "\" [label=\""
                        << label << "\"]\n";
        }
    }
    output_file << "}" << std::endl;

    output_file.close();
}

void FATools::to_dot(NFAState *initial_state, std::string output_path) {
    std::ofstream output_file;
    output_file.open(output_path);
    if (!output_file.is_open()) {
        std::string error_msg = "Failed to open file " + output_path;
        throw std::runtime_error(error_msg);
    }

    int next_node_number = 0;
    std::map<NFAState *, std::string> state2name;
    std::set<NFAState *> all_states = get_all_states(initial_state);

    output_file << "digraph graphname {\n";
    for (NFAState *state : all_states) {
        state2name[state] = "Node " + std::to_string(next_node_number++);
        output_file << "\t\"" << state2name[state] << "\"";
        if (state->is_accepting) {
            output_file << " [style=filled, fillcolor=green]";
        }
        output_file << "\n";
    }

    for (NFAState *state : all_states) {
        for (const auto &p : state->transitions) {
            const NFAInput &input = p.first;
            std::string label;
            if (input.is_epsilon)
                label = "epsilon";
            else
                label = "(" + std::to_string(input.dx) + ", " + std::to_string(input.dy) + ", " +
                        input.predicate->get_name() + ") {" + input.side_effect->get_name() + "}";
            const std::vector<NFAState *> &next_states = p.second;
            for (NFAState *next_state : next_states) {
                output_file << "\t\"" << state2name[state] << "\" -> \"" << state2name[next_state] << "\" [label=\""
                            << label << "\"]\n";
            }
        }
    }
    output_file << "}" << std::endl;

    output_file.close();
}

std::set<NFAState *> FATools::calculateEClosure(NFAState *state) {
    std::set<NFAState *> visited;
    std::queue<NFAState *> q;
    q.push(state);
    visited.insert(state);
    while (!q.empty()) {
        NFAState *at_state = q.front();
        q.pop();
        for (auto &p : at_state->transitions) {
            if (p.first.is_epsilon) {
                for (NFAState *next_state : p.second) {
                    if (visited.find(next_state) == visited.end()) {
                        q.push(next_state);
                        visited.insert(next_state);
                    }
                }
            }
        }
    }
    return visited;
}

std::set<NFAState *> FATools::calculateEClosure(std::set<NFAState *> states) {
    std::set<NFAState *> visited;
    for (NFAState *state : states) {
        std::set<NFAState *> merged;
        std::set<NFAState *> closure = calculateEClosure(state);
        std::merge(visited.begin(), visited.end(), closure.begin(), closure.end(), std::inserter(merged, merged.end()));
        visited = merged;
    }
    return visited;
}

std::set<NFAState *> FATools::move(std::set<NFAState *> states, NFAInput input) {
    std::set<NFAState *> visited;
    for (NFAState *state : states) {
        for (NFAState *next_state : state->transitions[input]) {
            visited.insert(next_state);
        }
    }
    return visited;
}

bool FATools::nfaSetContainsAcceptingState(std::set<NFAState *> states) {
    for (NFAState *state : states) {
        if (state->is_accepting)
            return true;
    }
    return false;
}

std::set<DFAInput> FATools::getAllInputSymbolsForSet(std::set<DFAState *> states) {
    std::set<DFAInput> input_symbols;
    for (DFAState *state : states) {
        for (auto &p : state->transition) {
            input_symbols.insert(p.first);
        }
    }
    return input_symbols;
}

std::set<NFAInput> FATools::getAllInputSymbolsForSet(std::set<NFAState *> states) {
    std::set<NFAInput> input_symbols;
    for (NFAState *state : states) {
        for (auto &p : state->transitions) {
            if (!p.first.is_epsilon) {
                input_symbols.insert(p.first);
            }
        }
    }
    return input_symbols;
}

std::unique_ptr<DFAState, DFAStateDeleter> FATools::nfaToDfa(NFAState *nfa_initial_state) {
    std::map<std::set<NFAState *>, DFAState *> nfa_set_to_dfa;
    std::set<NFAState *> nfa_initial_state_closure = calculateEClosure(nfa_initial_state);
    std::unique_ptr<DFAState, DFAStateDeleter> dfa_initial_state(new DFAState);
    nfa_set_to_dfa[nfa_initial_state_closure] = dfa_initial_state.get();
    nfa_set_to_dfa[nfa_initial_state_closure]->is_accepting = nfaSetContainsAcceptingState(nfa_initial_state_closure);
    std::queue<std::set<NFAState *>> q;
    q.push(nfa_initial_state_closure);
    while (!q.empty()) {
        std::set<NFAState *> at_nfa_set = q.front();
        DFAState *at = nfa_set_to_dfa[at_nfa_set];
        q.pop();
        for (NFAInput nfa_input : getAllInputSymbolsForSet(at_nfa_set)) {
            std::set<NFAState *> next_nfa_set = calculateEClosure(move(at_nfa_set, nfa_input));
            if (nfa_set_to_dfa.find(next_nfa_set) == nfa_set_to_dfa.end()) {
                nfa_set_to_dfa[next_nfa_set] = new DFAState();
                nfa_set_to_dfa[next_nfa_set]->is_accepting = nfaSetContainsAcceptingState(next_nfa_set);
                q.push(next_nfa_set);
            }
            DFAState *next_state = nfa_set_to_dfa[next_nfa_set];
            DFAInput dfa_input = DFAInput(nfa_input.dx, nfa_input.dy, nfa_input.predicate, nfa_input.side_effect);

            at->add_transition(next_state, dfa_input);
        }
    }
    return dfa_initial_state;
}

std::set<DFAState *> FATools::getParents(DFAState *child, std::set<DFAState *> *all_states, DFAInput input) {
    std::set<DFAState *> parents;
    for (DFAState *state : *all_states) {
        for (auto &p : state->transition) {
            if (p.second == child && p.first == input) {
                parents.insert(state);
            }
        }
    }
    return parents;
}

std::set<DFAState *> FATools::getParents(std::set<DFAState *> children, std::set<DFAState *> *all_states,
                                         DFAInput input) {
    std::set<DFAState *> all_parents;
    for (DFAState *child : children) {
        std::set<DFAState *> merged;
        std::set<DFAState *> parents = getParents(child, all_states, input);
        std::merge(all_parents.begin(), all_parents.end(), parents.begin(), parents.end(),
                   std::inserter(merged, merged.end()));
        all_parents = merged;
    }
    return all_parents;
}

std::unique_ptr<DFAState, DFAStateDeleter> FATools::minimizeDfa(DFAState *initial_state) {
    std::set<DFAState *> all_states = get_all_states(initial_state);
    std::set<DFAInput> alphabet = getAllInputSymbolsForSet(all_states);
    std::set<DFAState *> final_states;
    std::set<DFAState *> non_final_states;

    for (DFAState *state : all_states) {
        if (state->is_accepting)
            final_states.insert(state);
        else
            non_final_states.insert(state);
    }

    // https://en.wikipedia.org/wiki/DFA_minimization#Hopcroft's_algorithm
    std::set<std::set<DFAState *>> P{final_states, non_final_states};
    std::set<std::set<DFAState *>> W{final_states, non_final_states};
    while (!W.empty()) {
        std::set<DFAState *> A = *(W.begin());
        W.erase(W.begin());
        for (DFAInput input : alphabet) {
            std::set<DFAState *> X = getParents(A, &all_states, input);
            std::set<std::set<DFAState *>> newP;
            for (const std::set<DFAState *> &Y : P) {
                std::set<DFAState *> x_and_y;
                std::set_intersection(X.begin(), X.end(), Y.begin(), Y.end(), std::inserter(x_and_y, x_and_y.end()));
                if (x_and_y.empty()) {
                    newP.insert(Y);
                    continue;
                }
                std::set<DFAState *> y_minus_x;
                std::set_difference(Y.begin(), Y.end(), X.begin(), X.end(), std::inserter(y_minus_x, y_minus_x.end()));
                if (y_minus_x.empty()) {
                    newP.insert(Y);
                    continue;
                }
                newP.insert(x_and_y);
                newP.insert(y_minus_x);
                if (W.find(Y) != W.end()) {
                    W.erase(Y);
                    W.insert(x_and_y);
                    W.insert(y_minus_x);
                }
                else {
                    if (x_and_y.size() <= y_minus_x.size()) {
                        W.insert(x_and_y);
                    }
                    else {
                        W.insert(y_minus_x);
                    }
                }
            }
            P = newP;
        }
    }

    // Construct DFA from P
    std::map<std::set<DFAState *>, DFAState *> construction_map;
    DFAState *min_dfa_initial_state = nullptr;
    for (const std::set<DFAState *> &state_set : P) {
        construction_map[state_set] = new DFAState();
        if (!min_dfa_initial_state && state_set.find(initial_state) != state_set.end()) {
            min_dfa_initial_state = construction_map[state_set];
        }
        for (DFAState *state : state_set) {
            if (final_states.find(state) != final_states.end()) {
                construction_map[state_set]->is_accepting = true;
                break;
            }
        }
    }
    for (const std::set<DFAState *> &state_set : P) {
        DFAState *at_state = *(state_set.begin());
        for (auto &p : at_state->transition) {
            DFAInput input = p.first;
            DFAState *next_state = p.second;
            for (const std::set<DFAState *> &possible_next_set : P) {
                if (possible_next_set.find(next_state) != possible_next_set.end()) {
                    construction_map[state_set]->add_transition(construction_map[possible_next_set], input);
                    break;
                }
            }
        }
    }

    return std::unique_ptr<DFAState, DFAStateDeleter>(min_dfa_initial_state);
}

std::unique_ptr<DFAState, DFAStateDeleter> FATools::getMinimizedDfa(Node *node) {
    NFAVisitor nfaVisitor = NFAVisitor();
    node->accept(&nfaVisitor);
    std::unique_ptr<NFAState, NFAStateDeleter> nfa_initial_state = nfaVisitor.getNFA();
    std::unique_ptr<DFAState, DFAStateDeleter> dfa_initial_state = nfaToDfa(nfa_initial_state.get());
    std::unique_ptr<DFAState, DFAStateDeleter> min_dfa_initial_state = minimizeDfa(dfa_initial_state.get());
    return min_dfa_initial_state;
}
