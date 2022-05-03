#pragma once
#include <atomic>
#include <optional>
#include <inttypes.h>
#include "../Models/GameRules.hpp"
#include "../Models/Pawn.hpp"
#include "../Views/BasicPlayerPawnStyle.hpp"
#include "../Views/AnimatedPawnView.hpp"
#include "../Views/UI.hpp"

class DefaultHumanPlayer : // Handles user input as HumanPlayer, being game-actor (as AI) and provides a reference to a style for pawn creator
	public HumanPlayer
{
public:
	DefaultHumanPlayer(RenderWindow& window, PlayerPawnStyle& style)noexcept :
		selection_grid_(window),
		controller_(window, selection_grid_, checkerboard_view_),
		style_(style),
		checkerboard_view_(this)
	{}
	PlayerPawnStyle& get_pawn_style()noexcept final {
		return style_;
	}
	void start_game(Board& board, std::span<Pawn> own, std::span<Pawn> enemy) {
		board_ = &board;
		std::for_each(std::begin(own), std::end(own), [this](Pawn& pawn) {
			pawn.reset_owner(this);
		});
		own_pawns_ = own;
		enemy_pawns_ = enemy;
	}
	bool make_move()noexcept final {
		std::optional<board_cell> picked_cell = std::move(picked_cell_.load(std::memory_order_acquire));
		if (selected_pawn_) {
			if (picked_cell) {
				board_cell picked_position = *picked_cell, pawn_position = selected_pawn_->get_position();
				if (pawn_position == picked_position)
					reset_selection(picked_cell);
				else
					if (try_move(pawn_position, picked_position)) {
						reset_selection(picked_cell);
						return true;
					} else 
						select_pawn(picked_cell);
			}
		} else
			select_pawn(picked_cell);

		return false;
	}
	~DefaultHumanPlayer() = default;
public:
	void select_pawn(std::optional<board_cell>& picked_cell) {
		if (picked_cell) {
			board_cell position = *picked_cell;
			picked_cell.reset();
			picked_cell_.store(std::nullopt, std::memory_order_release);
			if (auto pawn = board_->get_pawn(position))
				if (is_my(*pawn))
					selected_pawn_ = pawn;
		}
	}
	bool try_move(board_cell pawn_position, board_cell picked_position) {
		if ((pawn_position.has_left_neighbour()) && (picked_position == pawn_position.get_left_neighbour()))
			return selected_pawn_->try_move_left(this);
		if ((pawn_position.has_right_neighbour()) && (picked_position == pawn_position.get_right_neighbour()))
			return selected_pawn_->try_move_right(this);
		if ((pawn_position.has_top_neighbour()) && (picked_position == pawn_position.get_top_neighbour()))
			return selected_pawn_->try_move_up(this);
		if ((pawn_position.has_bottom_neighbour()) && (picked_position == pawn_position.get_bottom_neighbour()))
			return selected_pawn_->try_move_down(this);
		return false;
	}
	void reset_selection(std::optional<board_cell>& picked_cell) {
		selected_pawn_ = nullptr;
		picked_cell.reset();
		picked_cell_.store(std::nullopt, std::memory_order_release);
	}
	void OnCellPicked(board_cell picked_cell)noexcept final {
		picked_cell_.store(picked_cell.masked_value(), std::memory_order_release);
	}
private:
	Pawn* selected_pawn_;
	std::atomic<std::optional<uint8_t>> picked_cell_;
	DefaultSelectionGrid selection_grid_;//TODO: interface
	DefaultBoardHumanView checkerboard_view_;//TODO: interface
	HumanPawnController controller_;//TODO: interface
	PlayerPawnStyle &style_;
	std::span<Pawn> own_pawns_, enemy_pawns_;
};
