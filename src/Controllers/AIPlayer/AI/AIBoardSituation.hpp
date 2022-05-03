#pragma once
#include <shared_mutex>
#include <numeric>
#include "../../../Models/Board.hpp"
#include "DescisionMaking/Minimax.hpp"
#include "DescisionMaking/AlternativeSelector.hpp"




class AIBoardSituation : public AISituation<AIPawnActionAlternative> {
	static constexpr player_index_t My = 0;
	static constexpr player_index_t Enemy = 1;
public:
	AIBoardSituation() = default;

	void start_game(Board& board, std::span<Pawn> my, std::span<Pawn> enemy);
	bool end_game()const final {
		return false;
	}

	int64_t evaluate(bool maximizer, int depth)noexcept final;

	//CollectInfoAboutMovablePawns()
	//PrioritizeMovablePawnInfos
	//return ConvertMovabalePawnInfosToAlternatives

	void on_enemy_have_made_move() {
		expected_board_state_ = actual_board_->get_state();
		for (auto& p : pawns_[My])
			p.desired_cell = p.pawn->get_position();
		for (auto& p : pawns_[Enemy])
			p.desired_cell = p.pawn->get_position();
	}

	std::vector<alternative_unique_ptr_t> get_possible_own_alternatives()final {
		return get_possible_alternatives(My, Enemy);
	}
	std::vector<alternative_unique_ptr_t> get_possible_enemy_alternatives()final {
		return get_possible_alternatives(Enemy, My);
	}

	std::unique_ptr<AISituation<AIPawnActionAlternative>> clone()const final {
		return std::unique_ptr<AISituation<AIPawnActionAlternative>>{ new AIBoardSituation(*this) };
	}

private:
	AIBoardSituation(const AIBoardSituation& src) {
		{
			std::shared_lock<decltype(mutex_)> lock(src.mutex_);
			std::copy(std::begin(src.goals_), std::end(src.goals_), std::begin(goals_));
			std::copy(std::begin(src.pawns_), std::end(src.pawns_), std::begin(pawns_));
			expected_board_state_ = src.expected_board_state_;
			actual_board_ = src.actual_board_;
		}
		for (auto& player_pawns : pawns_)
			for (auto& pawn : player_pawns)
				pawn.board_state_ = &expected_board_state_;
	}

	//TODO: to players???
	void start_player(player_index_t player, std::span<Pawn> player_pawns, std::span<Pawn> enemy_pawns) {
		for (uint8_t p = 0; p < std::size(pawns_[player]); ++p)
			pawns_[player][p].reset(expected_board_state_, player_pawns[p]);

		std::transform(std::begin(enemy_pawns), std::end(enemy_pawns), std::begin(goals_[player]), [](const Pawn& p)->board_cell {
			return p.get_position();
		});
	}
	//size_t count_of_movement_available(player_index_t )
	utitlity_t current_freedom_value(player_index_t player)const {
		return std::accumulate(std::begin(pawns_[player]), std::end(pawns_[player]), 0, [this, player](utitlity_t count, const AIPawnProxy& pawn)->utitlity_t {
			//for all that is not at goal better to be as free, as possible
			if (actual_board_->is_goal(player, pawn.desired_cell))
				return count;
			auto GetUtilityValueForSide = [player](bool can_move, player_index_t player_neighbour)->utitlity_t {
				if (can_move)
					return 1;
				if (player != player_neighbour)
					return -1;
				return 0;
			};
			return count + GetUtilityValueForSide(pawn.can_move_left(), expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_left_neighbour())) +
				GetUtilityValueForSide(pawn.can_move_up(), expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_top_neighbour())) +
				GetUtilityValueForSide(pawn.can_move_right(), expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_right_neighbour())) +
				GetUtilityValueForSide(pawn.can_move_down(), expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_bottom_neighbour()));
		});
	}
	utitlity_t max_freedom_value(player_index_t player)const {
		constexpr utitlity_t PotentialFreedomDirections = 4; // FreedomDirections are left, top, right and bottom 
		return PotentialFreedomDirections * std::count_if(std::begin(pawns_[player]), std::end(pawns_[player]), [this, player](const AIPawnProxy& pawn)->bool {
			return actual_board_->is_goal(player, pawn.desired_cell);
		}) - std::accumulate(std::begin(pawns_[player]), std::end(pawns_[player]), 0, [this, player](utitlity_t count, const AIPawnProxy& pawn)->bool {
			return (expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_left_neighbour()) == player) +
				(expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_right_neighbour()) == player) +
				(expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_top_neighbour()) == player) +
				(expected_board_state_.player_of_pawn_at(pawn.desired_cell.get_bottom_neighbour()) == player);
		});
	}


	size_t reached_goal_pawn_count(player_index_t player)const {
		return std::count_if(std::begin(pawns_[player]), std::end(pawns_[player]), [this, player](const AIPawnProxy& p)->bool {
			return (std::find(std::begin(goals_[player]), std::end(goals_[player]), p.desired_cell) != std::end(goals_[player]));
		});
	}
	size_t blocked_pawn_count(player_index_t player, std::span<board_cell> obstacles)const {
		return std::count_if(std::begin(pawns_[player]), std::end(pawns_[player]), [&obstacles](const AIPawnProxy& p)->bool {
			return !p.movable();
		});
	}
	float_t average_distance_to_goals(player_index_t player) {
		return std::accumulate(std::begin(pawns_[player]), std::end(pawns_[player]), 0.f, [this, player](float_t distace, const AIPawnProxy& p) {
			auto closest_goal = std::min_element(std::begin(goals_[player]), std::end(goals_[player]), [&p](board_cell l, board_cell r)->bool {
				return Distance(p.desired_cell, l) < Distance(p.desired_cell, r);
			});
			return distace + Distance(p.desired_cell, *closest_goal);//distace * Distance(p.desired_cell, *closest_goal);
		}) / float_t(std::size(pawns_[player]));
		//38443359375
		//
	}
private:
	std::vector<alternative_unique_ptr_t> get_possible_alternatives(player_index_t me, player_index_t enemy) {
		auto obstcales = GetBoardObstacles(pawns_[enemy], pawns_[me]);
		//auto infos = CollectInfoAboutMovablePawns(pawns_[me], goals_[me], obstcales);
		auto infos = MultithreadedCollectInfoAboutMovablePawns(pawns_[me], goals_[me], obstcales);
		PrioritizeMovablePawnInfos(infos);
		Expects(!std::empty(infos));
		return ConvertMovabalePawnInfosToAlternatives(infos, alternatives_factory_);
	}

private:
	AlternativesFactory alternatives_factory_;

	std::array<std::array<board_cell, GoalCountPerPlayer>, PlayerCount>	goals_;
	std::array<std::array<AIPawnProxy, PawnCountPerPlayer>, PlayerCount> pawns_;
	board_state expected_board_state_;

	Board* actual_board_;

private:
	mutable std::shared_mutex mutex_;
};