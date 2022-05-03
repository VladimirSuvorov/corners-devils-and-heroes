#pragma once
#include "GameRules.hpp"
#include "GameRules.hpp"
#include "Board.hpp"
class Player;
class Pawn {
public:
	// Constructor of a Pawn requires to have an onboard position and an owner. 
	// Owner required for 2 reasons: to get a player-owner, having a given pawn 
	// and to prevent moving pawns by non-owner players.
	Pawn(board_cell position, Player* owner = nullptr)noexcept :
		position_(position)
	{
		reset_owner(owner);
	}
	void reset_owner(Player* owner = nullptr)noexcept;
	const Player* get_owner()const noexcept {
		return owner_;
	}
	Player* get_owner()noexcept {
		return owner_;
	}

	// Pawn emplacement can fail if one attempts to emplace a pawn at position that already occupied by other one.
	success_t try_emplace_to(Board& checkerboard)noexcept {
		board_ = &checkerboard;
		return board_->try_emplace_pawn(*this);
	}
	board_cell get_position()const noexcept {
		return position_;
	}

	// Movement can fail in two cases:
	// 1) next cell is occupied by other pawn and/or doesn't exist,
	// 2) if non-owner player attempts to move a pawn that does'nt belong to him
	success_t try_move_left(const Player* commander = nullptr)noexcept {
		return try_move_to(position_.get_left_neighbour(), commander);
	}
	success_t try_move_right(const Player* commander = nullptr)noexcept {
		return try_move_to(position_.get_right_neighbour(), commander);
	}
	success_t try_move_up(const Player* commander = nullptr)noexcept {
		return try_move_to(position_.get_top_neighbour(), commander);
	}
	success_t try_move_down(const Player* commander = nullptr)noexcept {
		return try_move_to(position_.get_bottom_neighbour(), commander);
	}
	bool movable()const noexcept {
		return can_move_vertically() || can_move_horizontally();
	}
	bool can_move_vertically()const noexcept {
		return can_move_up() || can_move_down();
	}
	bool can_move_horizontally()const noexcept {
		return can_move_left() || can_move_right();
	}
	bool can_move_left()const noexcept {
		return can_move_to(position_.get_left_neighbour());
	}
	bool can_move_right()const noexcept {
		return can_move_to(position_.get_right_neighbour());
	}
	bool can_move_up()const noexcept {
		return can_move_to(position_.get_top_neighbour());
	}
	bool can_move_down()const noexcept {
		return can_move_to(position_.get_bottom_neighbour());
	}
private:
	bool can_move_to(board_cell position)const noexcept {
		if (!board_)
			return true;
		return board_->can_move_pawn(*this, position_, position);
	}
	success_t try_move_to(board_cell position, const Player* commander = nullptr) noexcept;
private:
	board_cell position_;
	Board* board_ = nullptr;
	Player* owner_;
	class PawnView* view_;//this is about how to display the pawn: Pawn is to be a game-pawn, where as PawnView to display it (SRP)
};
