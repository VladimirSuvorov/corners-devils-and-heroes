#include "Pawn.hpp"
#include "Board.hpp"
#include "../Controllers/Player.hpp"
#include "../utilities.h"

bool Board::all_goals_reached(Player& player)const {
	return !(current_state_.player_pawn_mask(player.get_index()) ^ goal_state_.player_pawn_mask(player.get_index()));
}
success_t Board::try_emplace_goal(Player& player, board_cell goal) {
	return goal_state_.try_emplace_pawn(player.get_index(), goal);
}

success_t Board::try_move_pawn(Pawn& pawn, board_cell from) {
	if (current_state_.try_move_pawn(from, pawn.get_position()))
		return true;
	current_state_.debug_display();
	return false;
}

Pawn* Board::get_pawn(board_cell at) {
	const player_index_t player_index = current_state_.player_of_pawn_at(at);
	if (player_index < std::size(players_)) {
		auto pawns = players_[player_index]->get_owned_pawns();
		auto found = std::find_if(std::begin(pawns), std::end(pawns), [at](const Pawn& p)->bool {
			return at == p.get_position();
		});
		if (found != std::end(pawns))
			return &(*found);
	}
	return nullptr;
}
bool Board::is_pawn_blocked(const Pawn& pawn)const {
	return current_state_.is_pawn_blocked(pawn.get_position());
}
bool Board::can_move_pawn(const Pawn& pawn, board_cell from, board_cell to)const {
	if (from == to)
		return false;

	return current_state_.can_move_pawn(from, to);
}

success_t Board::try_emplace_pawn(Pawn& pawn) {
	players_[pawn.get_owner()->get_index()] = pawn.get_owner();
	return current_state_.try_emplace_pawn(pawn.get_owner()->get_index(), pawn.get_position());
}