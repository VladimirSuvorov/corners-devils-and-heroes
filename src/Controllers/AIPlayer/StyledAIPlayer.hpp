#pragma once

#include "AIPlayer.hpp"
#include "../../Views/BasicPlayerPawnStyle.hpp"
#include "../../Views/AnimatedPawnView.hpp"


class StyledAIPlayer ://AI player, having refernece to a style
	public CrazyMadInsaneAIPlayer
{
public:
	StyledAIPlayer(PlayerPawnStyle& style)noexcept :
		style_(style)
	{}
	PlayerPawnStyle& get_pawn_style()noexcept final {
		return style_;
	}
private:
	PlayerPawnStyle& style_;
};
