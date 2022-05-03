#pragma once
class PlayerPawnStyle {
public:
	virtual class PawnView* acquire_unused_pawn_view() = 0;
	virtual void release_unused_pawn_view(class PawnView& view) = 0;
	virtual ~PlayerPawnStyle() = default;
};