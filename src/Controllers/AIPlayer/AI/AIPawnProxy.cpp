#include "AIPawnProxy.hpp"
alternative_unique_ptr_t AIPawnProxy::get_alternative(AlternativesFactory& alternatives_factory, board_cell next_cell) {
	Expects(desired_cell.is_neighbour(next_cell));
#if 0
	printf("--ALT-BOARD------\n");
	board_state_->debug_display();
	printf("pawn_desired_cell_->desired_cell { x = %d, y = %d }\n", int(desired_cell.x()), int(desired_cell.y()));
	printf("from_ { x = %d, y = %d }\n", int(desired_cell.x()), int(desired_cell.y()));
	printf("to_ { x = %d, y = %d }\n", int(next_cell.x()), int(next_cell.y()));
#endif

	if ((desired_cell.has_left_neighbour()) && (desired_cell.get_left_neighbour() == next_cell))
		return alternatives_factory.Left(*this, next_cell);
	if ((desired_cell.has_right_neighbour()) && (desired_cell.get_right_neighbour() == next_cell))
		return alternatives_factory.Right(*this, next_cell);
	if ((desired_cell.has_top_neighbour()) && (desired_cell.get_top_neighbour() == next_cell))
		return alternatives_factory.Up(*this, next_cell);
	if ((desired_cell.has_bottom_neighbour()) && (desired_cell.get_bottom_neighbour() == next_cell))
		return alternatives_factory.Down(*this, next_cell);
	return nullptr;
}