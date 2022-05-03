#include "AIPawnProxy.hpp"
#include "AIPawnActionAlternative.hpp"
template<typename Derived>
class PawnMoveAlternative : public AIPawnActionAlternative {
public:

	// Problem: after checking all alternatives, desired_cell.position points to a "random" place, but used to select Action, that fails because of "next pos = current pos"
	//desired cell is used to track state&position while minimax descision making, while a target_ - fixed - to make an action
	PawnMoveAlternative(AIPawnProxy& desired_cell, board_cell target, class AlternativesFactory& factory) :
		pawn_desired_cell_(desired_cell),
		from_(desired_cell.desired_cell),
		to_(target),
		factory_(&factory)
	{}
	PawnMoveAlternative(const PawnMoveAlternative&) = delete;
	PawnMoveAlternative& operator=(const PawnMoveAlternative&) = delete;
	PawnMoveAlternative(PawnMoveAlternative&& src) :
		pawn_desired_cell_(pawn_desired_cell_),
		from_(src.from_),
		to_(src.to_),
		performed_(src.performed_),
		factory_(src.factory_)
	{
		src.factory_ = nullptr;
	}
	PawnMoveAlternative& operator=(PawnMoveAlternative&& src) {
		pawn_desired_cell_ = pawn_desired_cell_;
		from_ = src.from_;
		to_ = src.to_;
		performed_ = src.performed_;
		factory_ = src.factory_;
		src.factory_ = nullptr;
		return *this;
	}
	void Try()final {
#if 0
		printf("Try\n");
		pawn_desired_cell_.board_state_->debug_display();
		printf("PawnPos haspawn = %s, left = %s right = %s top = %s bottom = %s\n",
			pawn_desired_cell_.board_state_->has_pawn(pawn_desired_cell_.desired_cell) ? "true" : "false",
			pawn_desired_cell_.board_state_->empty(pawn_desired_cell_.desired_cell.get_left_neighbour()) ? "true" : "false",
			pawn_desired_cell_.board_state_->empty(pawn_desired_cell_.desired_cell.get_right_neighbour()) ? "true" : "false",
			pawn_desired_cell_.board_state_->empty(pawn_desired_cell_.desired_cell.get_top_neighbour()) ? "true" : "false",
			pawn_desired_cell_.board_state_->empty(pawn_desired_cell_.desired_cell.get_bottom_neighbour()) ? "true" : "false"
		);
#endif
		//std::swap(pawn_desired_cell_.desired_cell, from_);
		static_cast<Derived*>(this)->Do();
#if 0
		printf("\n");
		pawn_desired_cell_.board_state_->debug_display();
		printf("End Try\n");
#endif
	}
	void Undo()final {
		//printf("Undo\n");
#if 0
		pawn_desired_cell_.board_state_->debug_display();
#endif
		static_cast<Derived*>(this)->UnDo();
		//printf("\n");
#if 0
		pawn_desired_cell_.board_state_->debug_display();
#endif
		//std::swap(pawn_desired_cell_.desired_cell, from_);
		//printf("End Undo\n");
	}
	void Redo()final {
		//printf("Redo\n");
#if 0
		pawn_desired_cell_.board_state_->debug_display();
#endif
		//std::swap(pawn_desired_cell_.desired_cell, from_);
		static_cast<Derived*>(this)->ReDo();
		//printf("\n");
#if 0
		pawn_desired_cell_.board_state_->debug_display();
#endif
		//std::swap(pawn_desired_cell_.desired_cell, from_);
		//printf("End Redo\n");
	}
protected:
	AIPawnProxy &pawn_desired_cell_;
	board_cell from_, to_;
	bool performed_ = false;
	class AlternativesFactory* factory_ = nullptr;
};
class PawnLeftMoveAlternative : public PawnMoveAlternative<PawnLeftMoveAlternative> {
public:
	PawnLeftMoveAlternative(AIPawnProxy& desired_cell, board_cell target, class AlternativesFactory& factory) :
		PawnMoveAlternative<PawnLeftMoveAlternative>(desired_cell, target, factory)
	{}
	void Do() {
		
		Expects(performed_ = pawn_desired_cell_.try_move_left());
	}
	void UnDo() {
		if (performed_)
			Expects(pawn_desired_cell_.try_move_right());
	}
	void ReDo() {
		Expects(pawn_desired_cell_.pawn->try_move_left(pawn_desired_cell_.pawn->get_owner()));
	}
};
class PawnRightMoveAlternative : public PawnMoveAlternative<PawnRightMoveAlternative> {
public:
	PawnRightMoveAlternative(AIPawnProxy& desired_cell, board_cell target, class AlternativesFactory& factory) :
		PawnMoveAlternative<PawnRightMoveAlternative>(desired_cell, target, factory)
	{}
	void Do() {
		Expects(performed_ = pawn_desired_cell_.try_move_right());
	}
	void UnDo() {
		if (performed_)
			Expects(pawn_desired_cell_.try_move_left());
	}
	void ReDo() {
		Expects(pawn_desired_cell_.pawn->try_move_right(pawn_desired_cell_.pawn->get_owner()));
	}
};
class PawnUpMoveAlternative : public PawnMoveAlternative<PawnUpMoveAlternative> {
public:
	PawnUpMoveAlternative(AIPawnProxy& desired_cell, board_cell target, class AlternativesFactory& factory) :
		PawnMoveAlternative<PawnUpMoveAlternative>(desired_cell, target, factory)
	{}
	void Do() {
		Expects(performed_ = pawn_desired_cell_.try_move_up());
	}
	void UnDo() {
		if (performed_)
			Expects(pawn_desired_cell_.try_move_down());
	}
	void ReDo() {
		Expects(pawn_desired_cell_.pawn->try_move_up(pawn_desired_cell_.pawn->get_owner()));
	}
};
class PawnDownMoveAlternative : public PawnMoveAlternative<PawnDownMoveAlternative> {
public:
	PawnDownMoveAlternative(AIPawnProxy& desired_cell, board_cell target, class AlternativesFactory& factory) :
		PawnMoveAlternative<PawnDownMoveAlternative>(desired_cell, target, factory)
	{}
	void Do() {

		Expects(performed_ = pawn_desired_cell_.try_move_down());
	}
	void UnDo() {
		if (performed_)
			Expects(pawn_desired_cell_.try_move_up());
	}
	void ReDo() {
		Expects(pawn_desired_cell_.pawn->try_move_down(pawn_desired_cell_.pawn->get_owner()));
	}
};

alternative_unique_ptr_t AlternativesFactory::Up(AIPawnProxy& actor, board_cell target) {
	return std::make_unique<PawnUpMoveAlternative>(actor, target, *this);
}
alternative_unique_ptr_t AlternativesFactory::Down(AIPawnProxy& actor, board_cell target) {
	return std::make_unique<PawnDownMoveAlternative>(actor, target, *this);
}
alternative_unique_ptr_t AlternativesFactory::Left(AIPawnProxy& actor, board_cell target) {
	return std::make_unique<PawnLeftMoveAlternative>(actor, target, *this);
}
alternative_unique_ptr_t AlternativesFactory::Right(AIPawnProxy& actor, board_cell target) {
	return std::make_unique<PawnRightMoveAlternative>(actor, target, *this);
}