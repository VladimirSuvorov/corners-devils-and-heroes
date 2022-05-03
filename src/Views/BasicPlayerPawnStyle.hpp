#pragma once
#include <memory>
#include "../Models/Pawn.hpp"
#include "../Graphics/GraphicTypes.hpp"
#include "PlayerPawnStyle.hpp"
#include "DefaultGameView.hpp"
#include "DefaultBoardVisualGrid.hpp"

template<typename ViewT, typename Allocator = std::allocator<ViewT>>
class BasicPlayerPawnStyle :
	public PlayerPawnStyle
{
public:
	BasicPlayerPawnStyle(DefaultGameView& game_view, AABB grid_origin_cell, AABB texture_region)noexcept :
		game_view_(game_view),
		board_grid_(grid_origin_cell),
		texture_region_(texture_region)
	{}
	PawnView* acquire_unused_pawn_view()noexcept final {
		auto view = allocator_.allocate(1);
		std::allocator_traits<Allocator>::construct(allocator_, view, game_view_.create<ViewT>(board_grid_, texture_region_));
		return view;
	}
	void release_unused_pawn_view(PawnView& view)noexcept final {
		std::allocator_traits<Allocator>::destroy(allocator_, &view);
		allocator_.deallocate(static_cast<ViewT*>(&view), 1);
	}
	~BasicPlayerPawnStyle() = default;
private:
	AABB texture_region_;
	Allocator allocator_;
	DefaultGameView& game_view_;
	TiledBoardVisualGrid board_grid_;
};

