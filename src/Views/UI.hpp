#pragma once
#include "../Models/GameRules.hpp"
#include "../Window/RenderWindow.hpp"
#include "../Controllers/Player.hpp"
#include "../Controllers/HumanPawnController.hpp"

class DefaultSelectionGrid :// DefaultSelectionGrid is class to convert mouse coordinates to board_cell
	public SelectionGrid
{
public:
	DefaultSelectionGrid(const RenderWindow& w)noexcept :
		window_(w)
	{}

	board_cell* cell(double_t relative_to_window_left_x, double_t relative_to_window_top_y)const noexcept {
		auto [width, height] = window_.size();
		position_.xy(relative_to_window_left_x * 8 / double_t(width), relative_to_window_top_y * 8 / double_t(height));
		return &position_;
	}
private:
	mutable board_cell position_;
	const RenderWindow& window_;
};

class HumanPlayer :// HumanPlayer is exactly same as Player, except it reacts on clicking
	public Player
{
public:
	virtual void OnCellPicked(board_cell picked_cell)noexcept = 0;

};

class DefaultBoardHumanView :// DefaultBoardHumanView is for convertation
	public BoardHumanView
{
public:
	void OnCellUnderCursor(board_cell* cell_position)noexcept {}

	void OnCellPicked(board_cell* cell_position)noexcept {
		if (cell_position) {
			board_cell position = *cell_position;
			if (owner_)
				owner_->OnCellPicked(position);
		}
	}
public:
	DefaultBoardHumanView(HumanPlayer* owner = nullptr) :
		owner_(owner)
	{}
private:
	HumanPlayer* owner_ = nullptr;
};