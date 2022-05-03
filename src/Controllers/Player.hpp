#pragma once
#include <array>
#include <gsl/gsl>
#include "../Models/Pawn.hpp"
#include "../utilities.h"
#include "../Models/GameRules.hpp"

template<uint8_t PawnCount = 9>
class BasicPlayer {//Controller
public:
	// Virtuals
	virtual bool make_move() = 0;// how to make a move (and mostly - will player do that?)
	virtual class PlayerPawnStyle& get_pawn_style() = 0; // how to display pawns
	virtual ~BasicPlayer() = default;


	// Common
	void start_game(player_index_t player_index, class Board& board, std::span<const board_cell> initial_positions, std::span<const board_cell> goals);
	player_index_t get_index()const noexcept {
		return own_player_index_;
	}
	void remove_pawns() {
		for (auto& pawn : pawns_)
			pawn.destroy();
	}
	void check_goals_reached_or_blocked()noexcept {
		//win if UNABLE to find a pawn that is OUT OF GOAL
		
		goals_reached_ = board_->all_goals_reached(*static_cast<Player*>(this));

		//blocked if there is NO MOVABLE pawns
		blocked_ = (std::find_if(std::begin(pawns_), std::end(pawns_), [](const deferred<Pawn>& p)->bool {
			return p->movable();
		}) == std::end(pawns_));
	}
	bool goals_reached()const noexcept {
		return goals_reached_;
	}
	bool blocked()const noexcept {
		return blocked_;
	}
	bool is_my(const Pawn& pawn)const noexcept {
		return (pawn.get_owner() == this) ? true : (std::find_if(std::begin(pawns_), std::end(pawns_), [this, &pawn](const deferred<Pawn>& p)->bool {
			return (&pawn == p.as_pointer()) || (pawn.get_owner() == p->get_owner());
		}) != std::end(pawns_));
	}

	std::span<Pawn> get_owned_pawns() {
		return { pawns_.front().as_pointer(), std::size(pawns_) };
	}
	std::span<const Pawn> get_owned_pawns()const noexcept {
		return { pawns_.front().as_pointer(), std::size(pawns_) };
	}

	virtual void OnStartGame(Board& board, std::span<Pawn> own, std::span<Pawn> enemy) {}
protected:
	std::array<deferred<Pawn>, PawnCount> pawns_;
	Board* board_;
private:
	
private:

	player_index_t own_player_index_ = 0;
	//blocked_ - case that can occur because of ability to move 1 cell forth and back using same pawn.
	//Therefore, there is possible situation that one player can "block" all pawns of other player at some move,
	//so a current turn player (blocked on previous turn by move of other player) will be UNable to make a move.
	//
	//	...................
	//	...	[ ][x][ ][ ][ ]	
	//	...	[x][x][x][x][x]
	//	...	[ ][x][o][o][o]
	//	...	[ ][x][o][o][o]
	//	...	[ ][x][o][o][o]
	//
	//	x, o - pawns of 2 players respectively
	bool goals_reached_ = false, blocked_ = false;
	std::array<board_cell, PawnCount> initial_positions_;
	std::array<board_cell, PawnCount> goals_;
};

// Player is forward declared in a set of files as a 'class' - not a 'TEMPLATE class'.
// To don't rewrite all these declarations, fake inheritance is used.)
class Player : public BasicPlayer<9> 
{};