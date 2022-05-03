#include "../Controllers/Player.hpp"
#include "Pawn.hpp"
#include "../Views/PlayerPawnStyle.hpp"
#include "../Views/PawnView.hpp"
void Pawn::reset_owner(Player* owner)noexcept {
	if (owner_ == owner)
		return;
	if (owner_ && view_)
		owner_->get_pawn_style().release_unused_pawn_view(*view_);
	if (owner_ = owner)
		if (view_ = owner_->get_pawn_style().acquire_unused_pawn_view())
			view_->set_pawn_position(position_);
}
success_t Pawn::try_move_to(board_cell position, const Player* commander)noexcept {
	if (owner_ != commander)
		return false;
	if (!board_) {
		if (view_)
			view_->on_pawn_position_changed(position_, position);
		position_ = position;
	} else {
		auto prev_position = position_;
		position_ = position;
		if (!board_->try_move_pawn(*this, prev_position)) {
			position_ = prev_position;
			return false;
		}
		if (view_)
			view_->on_pawn_position_changed(prev_position, position_);
	}
	return true;
}