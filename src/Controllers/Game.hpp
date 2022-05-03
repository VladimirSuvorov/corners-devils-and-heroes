#pragma once
#include <array>
#include "../Models/Pawn.hpp"
#include "../Models/Board.hpp"
#include "../Models/GameRules.hpp"
#include "../Models/GameRules.hpp"
#include "Player.hpp"
#include "../utilities.h"

// The core of the game. 
// This class is responsible for the game "in whole".
// Mainly, it is responsible for transferring a turn between players and checking for win-lose conditions for them both.
class Game {// Controller
public:
	Game(std::span<Player*> players) {
		players = std::span<Player*>{ std::data(players), std::min(std::size(players), std::size(players_)) };
		std::copy(std::begin(players), std::end(players), std::begin(players_));
		on_restart(false);
	}
	void restart() {
		on_restart(true);
	}


	success_t turn_forward() {
		if (!game_finished()) {
			if (!players_[current_turn_player_index_]->make_move())
				return false;
			else {
				pass_next_turn_to_next_player();
				check_for_win_fail_conditions();
				return true;
			} 
			Expects(false);
		}
		return false;
	}

	bool game_finished()const {
		return get_winner() || current_player_blocked() || all_enemies_are_blocked();
	}

	bool current_player_blocked()const noexcept {
		return players_[current_turn_player_index_]->blocked();
	}

	Player* get_winner()const noexcept {
		auto found = std::find_if(std::begin(players_), std::end(players_), [](Player* p)->bool {
			return p->goals_reached();
		});
		return (found != std::end(players_)) ? *found : nullptr;
	}
	bool all_enemies_are_blocked()const noexcept {
		for (player_index_t p = 0; p < current_turn_player_index_; ++p)
			if (!players_[p]->blocked())
				return false;
		for (player_index_t p = current_turn_player_index_ + 1; p < std::size(players_); ++p)
			if (!players_[p]->blocked())
				return false;
		return true;
	}
private:
	void pass_next_turn_to_next_player() {
		if (!((++current_turn_player_index_) < std::size(players_)))
			current_turn_player_index_ = 0;
	}
	void check_for_win_fail_conditions();
	void on_restart(bool is_restart);


private:
	player_index_t current_turn_player_index_ = 0;
	Board board_;
	std::array<Player*, PlayerCount> players_;
};

