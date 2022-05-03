#pragma once
#include "DescisionMaking/Minimax.hpp"

class AIPawnActionAlternative {
public:
	using deleter_type = std::default_delete<AIPawnActionAlternative>;

	virtual void Try() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;

	virtual ~AIPawnActionAlternative() = default;

};

using alternative_unique_ptr_t = alternative_unique_ptr<AIPawnActionAlternative>;



class AlternativesFactory {
public:
	alternative_unique_ptr_t Up(struct AIPawnProxy& actor, board_cell target);
	alternative_unique_ptr_t Down(struct AIPawnProxy& actor, board_cell target);
	alternative_unique_ptr_t Left(struct AIPawnProxy& actor, board_cell target);
	alternative_unique_ptr_t Right(struct AIPawnProxy& actor, board_cell target);
};




