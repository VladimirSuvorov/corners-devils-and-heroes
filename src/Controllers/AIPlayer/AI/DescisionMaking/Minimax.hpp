#pragma once
#include <type_traits>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <utility>


template<typename Alternative>
using alternative_unique_ptr = std::unique_ptr<Alternative, typename Alternative::deleter_type>;

using utitlity_t = int64_t;

template<typename Alternative>
class AISituation {//A game situation from AI point of view is just a set of alternative actions, total game score of that situation and end-game condition
public:	

	virtual std::vector<alternative_unique_ptr<Alternative>> get_possible_own_alternatives() = 0;
	virtual std::vector<alternative_unique_ptr<Alternative>> get_possible_enemy_alternatives() = 0;

	virtual utitlity_t evaluate(bool maximizer, int depth) = 0;//returns win, loose and game_end
	virtual bool end_game()const = 0;

	// clone - is unrelated to AISituation itself, but is used (can be) for multitheading
	virtual std::unique_ptr<AISituation<Alternative>> clone()const = 0;
};


//parameters for minimax
constexpr utitlity_t InitialMaximizerValue = -1000000000;
constexpr utitlity_t InitialMinimizerValue = 1000000000;
constexpr utitlity_t DepthPenalty = 10;
constexpr size_t MaxMinimaxThreadCount = 3;
constexpr auto MaxMinimaxDepth = 500; //5;//3;//4;// 1; // 3;

template<typename Alternative>// typical minimax algorithm implementation
utitlity_t Minimax(uint32_t depth = 0, bool maximizer = false, utitlity_t max_best_score = InitialMaximizerValue, utitlity_t min_best_score = InitialMinimizerValue, AISituation<Alternative>* current_situation = nullptr);
template<typename Alternative>// same as Minimax, but at depth == 0 launches several threads for each block of alternatives
utitlity_t MultithreadedMinimax(bool maximizer, utitlity_t max_best_score, utitlity_t min_best_score, AISituation<Alternative>* current_situation);

#include "Minimax.inl"
