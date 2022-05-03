#include "../Models/Pawn.hpp"
#include "Game.hpp"


void Game::check_for_win_fail_conditions() {
	auto NoGoalWithThisPawn = [](const Pawn& pawn, const std::array<board_cell, 9>& player_goals) {
		return std::find(std::begin(player_goals), std::end(player_goals), pawn.get_position()) == std::end(player_goals);
	};
	for (auto& player : players_)
		player->check_goals_reached_or_blocked();
}
void Game::on_restart(bool is_restart) {
	if (is_restart) {
		for (auto& player : players_)
			player->remove_pawns();
		board_.clear();
	}

	current_turn_player_index_ = 0;

	for (player_index_t p = 0; p < std::size(players_); ++ p)
		players_[p]->start_game(p, board_, GameBoard::GetInitialPlayerPawnPositions(p), GameBoard::GetPlayerPawnGoalPositions(p));


	for (player_index_t p = 0; p < std::size(players_); ++p)
		players_[p]->OnStartGame(board_, players_[p]->get_owned_pawns(), players_[(std::size(players_) - 1) - p]->get_owned_pawns());
}