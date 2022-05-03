#pragma once
#include "../../../Models/GameRules.hpp"
#include "../../../Models/Pawn.hpp"
#include "../../Player.hpp"
#include "AIPawnActionAlternative.hpp"
using success_t = bool;
struct AIPawnProxy {
	Pawn* pawn = nullptr;//actually, pawn performs no move - until the best move would be found, hovever, this pointer is used to perform an actual action, leading the pawn to desired_cell
	board_cell desired_cell;//where the pawn would be after several steps forward on a given turn
	board_state* board_state_ = nullptr;

	void reset(board_state& board_state, Pawn& p) {
		pawn = &p;
		desired_cell = p.get_position();
		if (!(board_state_ = &board_state)->try_emplace_pawn(p.get_owner()->get_index(), desired_cell)) {
			printf("pos: %d %d\n", int(desired_cell.x()), int(desired_cell.y()));
		}
	}

	alternative_unique_ptr_t get_alternative(class AlternativesFactory& alternatives_factory, board_cell neighbour_cell);

	success_t try_move_left()noexcept {
		if (can_move_left())
			if (board_state_->try_move_pawn(desired_cell, desired_cell.get_left_neighbour())) {
				desired_cell = desired_cell.get_left_neighbour();
				return true;
			}
		return false;
	}
	success_t try_move_right()noexcept {
		if (can_move_right())
			if (board_state_->try_move_pawn(desired_cell, desired_cell.get_right_neighbour())) {
				desired_cell = desired_cell.get_right_neighbour();
				return true;
			}
		return false;
	}
	success_t try_move_up()noexcept {
		if (can_move_up())
			if (board_state_->try_move_pawn(desired_cell, desired_cell.get_top_neighbour())) {
				desired_cell = desired_cell.get_top_neighbour();
				return true;
			}
		return false;
	}
	success_t try_move_down()noexcept {
		if (can_move_down())
			if (board_state_->try_move_pawn(desired_cell, desired_cell.get_bottom_neighbour())) {
				desired_cell = desired_cell.get_bottom_neighbour();
				return true;
			}
		return false;
	}
	bool can_move(board_cell from, board_cell to)const noexcept {
		if (board_state_->can_move_pawn(from, to))
			return true;
		return false;
	}

	bool can_move_left()const noexcept {
		return (desired_cell.has_left_neighbour()) ? can_move(desired_cell, desired_cell.get_left_neighbour()) : false;
	}
	bool can_move_right()const noexcept {
		return (desired_cell.has_right_neighbour()) ? can_move(desired_cell, desired_cell.get_right_neighbour()) : false;
	}
	bool can_move_up()const noexcept {
		return (desired_cell.has_top_neighbour()) ? can_move(desired_cell, desired_cell.get_top_neighbour()) : false;
	}
	bool can_move_down()const noexcept {
		return (desired_cell.has_bottom_neighbour()) ? can_move(desired_cell, desired_cell.get_bottom_neighbour()) : false;
	}

	bool movable_horizontally()const {
		return can_move_left() || can_move_right();
	}
	bool movable_vertically()const {
		return can_move_up() || can_move_down();
	}
	bool movable()const {
		return movable_horizontally() || movable_vertically();
	}
};
