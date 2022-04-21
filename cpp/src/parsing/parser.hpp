// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2022 Bjarni Dagur Thor Kárason <bjarni@bjarnithor.com>
 */
/**
 *  @file parser.hpp
 *  @brief Defines a Parser for the game description language.
 *  @author Bjarni Dagur Thor Kárason
 */
#pragma once

#include "ast.hpp"
#include "dfa.hpp"
#include "environment.hpp"
#include "fa_tools.hpp"
#include "lexer.hpp"
#include "macro_visitor.hpp"
#include <memory>
#include <set>
#include <sstream>
#include <utility>

/// @brief A class that parses a game description.
/// @details
///  Reads a tokenized game description and creates a game Environment from it.
///  To be used to Lexer.
///
/// @see Lexer
///
/// @author Bjarni Dagur Thor Kárason
class Parser
{
  public:
    /// @brief Parser constructor.
    ///
    /// @param file_path a path to a file containing the game description to parse.
    Parser(std::string file_path);
    /// @brief Parser deconstructor.
    ~Parser();
    /// @brief Parses the game description.
    /// @details
    ///  Resolves macros, creates state machines for each piece, and creates a
    ///  game Environment from the game description.
    void parse();
    /// @brief Returns the parsed game environment.
    ///
    /// @note Since the Environment is returned as a std::unique_ptr this
    ///  function can only be called once.
    ///
    /// @note Calling this function also resets the Parser#pieces and
    ///  Parser#post_conditions variables.
    ///
    /// @returns a pointer to the parsed game environment.
    std::unique_ptr<Environment> get_environment();

  private:
    /// @brief The Parser's tokenizer.
    Lexer lexer;
    /// @brief The current Token as a TokenTuple.
    TokenTuple tokenTuple;
    /// @brief The players defined in the game description.
    std::vector<std::string> players;
    /// @brief The pieces defined in the game description, which player they
    ///  belong to, and how they can move.
    /// @details
    ///  A map that takes a piece's name and returns the name of the player it
    ///  belongs to, and a state machine to generate legal moves for it.
    std::map<std::string, std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>> pieces;
    /// @brief The macros defined in the game description, and the names of their arguments.
    /// @details
    ///  A map that takes a macro's name and returns the root of its abstract
    ///  syntax tree, and a vector containing the names of its arguments.
    ///
    /// @see Node
    std::map<std::string, std::pair<std::unique_ptr<Node>, std::vector<std::string>>> macros;
    /// @brief The post conditions defined in the game description that must
    ///  hold after a player makes a move.
    /// @details
    ///  A map that takes a player's name and returns a vector containing all of
    ///  his post conditions as piece/state machine pairs.
    ///
    /// @note
    ///  Post conditions are defined for player/piece pairs. After the player
    ///  makes a move, all his post conditions are checked for each of the
    ///  pieces they are defined for. Post conditions are defined using regular
    ///  expressions, and a post condition holds if the corresponding regular
    ///  expression is not matched.
    std::map<std::string, std::vector<std::pair<std::string, std::unique_ptr<DFAState, DFAStateDeleter>>>>
        post_conditions;
    /// @brief A pointer to the Environment parsed from the game description.
    ///
    /// @see Environment
    std::unique_ptr<Environment> environment;
    /// @brief Matches the next token in the token stream.
    ///
    /// @param token the Token to match.
    ///
    /// @throw std::runtime_error if the Token to match and the next Token in
    ///  the token stream do not match.
    void match(Token token);
    /// @brief Tries to match the next token in the token stream.
    /// @details
    ///  Only matches the next token in the token stream if it is possible.
    ///
    /// @param token the Token to match.
    ///
    /// @returns true if the match was successful
    /// @return false if the match was not successful
    bool match_if(Token token);
    /// @brief Extracts the player names declared in the game description.
    /// @details
    ///  Stores the player names in Parser#players.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if players have already been declared.
    /// @throw std::runtime_error if duplicate players are declared.
    void parse_player_list();
    /// @brief Extracts the piece names and the players they belong to from the
    ///  game description.
    /// @details
    ///  Stores the information in Parser#pieces.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Parser::parse_piece which can throw errors.
    /// @throw std::runtime_error if pieces have already been declared.
    void parse_pieces_list();
    /// @brief Extracts a single piece and the player it belongs to from the
    ///  game description.
    /// @details
    ///  Stores the information in Parser#pieces.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Parser::parse_piece which can throw errors.
    /// @throw std::runtime_error if a piece belongs to an undeclared player.
    /// @throw std::runtime_error if duplicate pieces are defined.
    void parse_piece();
    /// @brief Extracts the board size from the game description.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if board size has already been declared.
    /// @throw std::runtime_error if board size is not positive.
    void parse_board_size();
    /// @brief Extracts the initial board configuration from the game
    ///  description.
    /// @details
    ///  Stores the information in Parser#environment.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if the board size has not been declared.
    /// @throw std::runtime_error if an undeclared piece is placed on the board.
    /// @throw std::runtime_error if player's piece with no associated rule is placed on the board.
    /// @throw std::runtime_error if too many/too few pieces are placed on the board.
    void parse_board();
    /// @brief Extract how a piece can move from the game description.
    /// @details
    ///  Converts the regular expression describing piece movement to a state machine.
    ///
    /// @see DFAState
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if declaring a rule for an undeclared piece.
    /// @throw std::runtime_error if the piece already has a declared rule.
    void parse_rule();
    /// @brief Extract a macro definition from the game description.
    /// @details
    ///  Converts the macro definition to an abstract syntax tree and stores it
    ///  in Parser#macros.
    ///
    /// @see Node
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Token::parse_arguments which can throw errors.
    /// @warning Uses Token::parse_sentence which can throw errors.
    void parse_macro();
    /// @brief Extracts a macro's arguments names/values.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Token::parse_argument which can throw errors.
    ///
    /// @returns a vector containing a macro's argument names/values.
    std::vector<std::string> parse_arguments();
    /// @brief Extracts a single macro argument name/value.
    ///
    /// @warning Uses Token::match which can throw errors.
    ///
    /// @returns the argument name/value.
    std::string parse_argument();
    /// @brief Extract a post condition for a player/piece pair.
    /// @details
    ///  Stores the information in Parser#post_conditions.
    ///
    /// @see Parser#post_conditions
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if declaring a post condition for an undeclared player.
    /// @throw std::runtime_error if declaring a post condition for an undeclared piece.
    void parse_post_condition();
    /// @brief Expands a macro call in the game description.
    /// @details
    ///  Returns the root of the macro's abstract syntax tree (a copy) with the
    ///  correct argument values.
    ///
    /// @param in_macro true if macro is a being called from another macro,
    ///  false otherwise.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if calling undefined macro.
    /// @throw std::runtime_error if invalid number of arguments.
    ///
    /// @returns a pointer to the root of the macro's abstract syntax tree (a
    ///  copy) with correct argument values.
    std::unique_ptr<Node> parse_macro_call(bool in_macro);
    /// @brief Parses a regular expression from the game description.
    ///
    /// @param in_macro true if the expression being parsed is a part of a macro
    ///  definition, false otherwise.
    ///
    /// @note Refer to the language grammar for more details.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Token::parse_word which can throw errors.
    ///
    /// @returns a pointer to the root of the expression's abstract syntax tree.
    std::unique_ptr<Node> parse_sentence(bool in_macro = false);
    /// @brief Parses a regular expression word from the game description.
    ///
    /// @param in_macro true if the expression being parsed is a part of a macro
    ///  definition, false otherwise.
    ///
    /// @note Refer to the language grammar for more details.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Token::parse_macro_call which can throw errors.
    /// @warning Uses Token::parse_core_word which can throw errors.
    ///
    /// @returns a pointer to the root of the expression's abstract syntax tree.
    std::unique_ptr<WordsNode> parse_word(bool in_macro);
    /// @brief Parses a regular expression core word from the game description.
    ///
    /// @param in_macro true if the expression being parsed is a part of a macro
    ///  definition, false otherwise.
    ///
    /// @note Refer to the language grammar for more details.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @warning Uses Token::parse_sentence which can throw errors.
    /// @warning Uses Token::parse_letter which can throw errors.
    /// @warning Uses Token::parse_macro_letter which can throw errors.
    ///
    /// @returns a pointer to the root of the expression's abstract syntax tree.
    std::unique_ptr<Node> parse_core_word(bool in_macro);
    /// @brief Parses a regular expression letter from the game description.
    ///
    /// @note Refer to the language grammar for more details.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if using an undefined predicate.
    /// @throw std::runtime_error if using an undefined side effect.
    ///
    /// @see Predicate
    /// @see SideEffect
    ///
    /// @returns a pointer to the root of the expression's abstract syntax tree.
    std::unique_ptr<LetterNode> parse_letter();
    /// @brief Parses a macro'd regular expression letter from the game description.
    ///
    /// @note Refer to the language grammar for more details.
    ///
    /// @warning Uses Token::match which can throw errors.
    /// @throw std::runtime_error if using an undefined predicate.
    /// @throw std::runtime_error if using an undefined side effect.
    ///
    /// @see Predicate
    /// @see SideEffect
    ///
    /// @returns a pointer to the root of the expression's abstract syntax tree.
    std::unique_ptr<MacroLetterNode> parse_macro_letter();
    /// @brief Parses a number of the game description.
    ///
    /// @warning Uses Token::match which can throw errors.
    ///
    /// @returns the parsed number.
    int parse_int();
    /// @brief Parses a string of the game description.
    ///
    /// @warning Uses Token::match which can throw errors.
    ///
    /// @returns the parsed std::string.
    std::string parse_string();
};
