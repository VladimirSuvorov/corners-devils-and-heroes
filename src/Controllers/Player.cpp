#include "../Models/Board.hpp"
#include "Player.hpp"


void BasicPlayer<9>::start_game(player_index_t player_index, Board& board, std::span<const board_cell> initial_positions, std::span<const board_cell> goals) {
	own_player_index_ = player_index;
	board_ = &board;
	initial_positions = std::span<const board_cell>{ std::data(initial_positions), std::min(std::size(initial_positions), std::size(initial_positions_)) };
	goals = std::span<const board_cell>{ std::data(goals), std::min(std::size(goals), std::size(goals_)) };
	std::copy(std::begin(initial_positions), std::end(initial_positions), std::begin(initial_positions_));
	std::copy(std::begin(goals), std::end(goals), std::begin(goals_));

	auto player_this = static_cast<Player*>(this);
	for (uint8_t p = 0; p < std::min(std::size(pawns_), std::size(initial_positions_)); ++p) {
		pawns_[p].create(initial_positions_[p], player_this);
		if (!pawns_[p]->try_emplace_to(*board_))
			LOG_INFO("Unable to push pawn to the board\n");
	}
	for (auto& g : goals_)
		board_->try_emplace_goal(*player_this, g);
}