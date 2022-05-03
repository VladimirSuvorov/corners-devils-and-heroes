#pragma once
#include "../Models/GameRules.hpp"
#include "../Window/RenderWindow.hpp"
// В данном файле реализация управления для игрока-человека.


class SelectionGrid {
public:
	// May return nullptr if no cell selected
	virtual board_cell* cell(double_t relative_to_window_left_x, double_t relative_to_window_top_y)const = 0;
};

class BoardHumanView {
public:
	// nullptr - is possible value (as raw pointer is used instead of gsl::not_null<T>)
	virtual void OnCellUnderCursor(board_cell *position) = 0;
	virtual void OnCellPicked(board_cell *position) = 0;
};


class HumanPawnController :// Used to pick a cell or a pawn in game. Converts mouse clicks to board cell, using SelectionGrid and BoardHumanView
	public MouseEventHandler {
public:
	HumanPawnController(RenderWindow& window, SelectionGrid& selection_grid, BoardHumanView& view) :
		selection_grid_(&selection_grid),
		view_(&view)
	{
		window.set_mouse_event_handler(this);
	}
	virtual void OnMouseMove(double_t x, double_t y) {
		current_mouse_x_ = x;
		current_mouse_y_ = y;
		view_->OnCellUnderCursor(selection_grid_->cell(current_mouse_x_, current_mouse_y_));
	}
	virtual void OnButtonChangesState(int mouse_button_index, bool pressed) {
		if ((mouse_button_index == 0) && pressed)
			view_->OnCellPicked(selection_grid_->cell(current_mouse_x_, current_mouse_y_));
	}
private:
	BoardHumanView* view_;
	SelectionGrid* selection_grid_;
	double_t current_mouse_x_, current_mouse_y_;
};