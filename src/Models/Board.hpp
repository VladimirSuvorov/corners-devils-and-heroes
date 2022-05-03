#pragma once
#include <map>
#include "GameRules.hpp"


class Pawn;
class Player;
// Board class represents the game board with a logic related to the board.

class Board {//Todo: board checks if a pawn reached something
public:
	// emplace_* methods can fail, if some position or pawn already emplaced
	success_t try_emplace_pawn(Pawn& pawn);
	success_t try_move_pawn(Pawn& pawn, board_cell to);

	success_t try_emplace_goal(Player& player, board_cell goal); 
	bool all_goals_reached(Player& player)const ;


	bool can_move_pawn(const Pawn& pawn, board_cell from, board_cell to)const;
	//the pawn is blocked if there is no direction (left, up, right, down) where it can be moved.
	bool is_pawn_blocked(const Pawn& pawn)const;
	void clear() {
		current_state_.clear();
		memset(std::data(players_), 0, std::size(players_) * sizeof(decltype(players_)::value_type));
	}
	bool is_goal(player_index_t player, board_cell pos)const {
		return goal_state_.has_player_pawn(player, pos);
	}
	Pawn* get_pawn(board_cell at);

	const Pawn* get_pawn(board_cell at)const {
		return const_cast<Board*>(this)->get_pawn(at);
	}

	board_state get_state() {
		return current_state_;
	}
	std::span<Player*> get_players() {
		return players_;
	}
private:

	// An std::array<Pawn*, 64> is about sizeof(ptr) size.
	// A std::map<board_cell, Pawn*> is not in contiguous memory.
	//
	// In comparison: board_state is about 2 bytes (and testible fast because of bitmasks) + all pawns are stored contiguously in player class.
	board_state current_state_, goal_state_;
	std::array<Player*, PlayerCount> players_;//board_state doesn't store a pointers to the players. So, they are stored separately.
};
