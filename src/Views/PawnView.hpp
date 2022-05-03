#pragma once
#include "../Models/GameRules.hpp"
class PawnView {
public:
	virtual void set_pawn_position(board_cell position) = 0;
	virtual void on_pawn_position_changed(board_cell from, board_cell to) = 0;
	//does changing pawn position update it's view? (gives an ability to temporary disable view update)
	virtual void set_move_update_enabled(bool enabled) = 0;
	virtual ~PawnView() = default;
};
