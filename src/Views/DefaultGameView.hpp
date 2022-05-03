#pragma once
#include "../Graphics/Drawable.hpp"
#include "GameView.hpp"
#include "TiledBoardView.hpp"

class DefaultGameView :
	public GameView
{
public:
	DefaultGameView(TiledBoardView& background) {
		root_view_group_.attach(background);
	}
	void set_background(TiledBoardView& background) {
		root_view_group_.attach(background);
	}
	void attach_drawable(Drawable& drawable) {
		root_view_group_.attach(drawable);
	}
	void detach_drawable(Drawable& drawable) {
		root_view_group_.detach(drawable);
	}

	const std::vector<quad>* GetQuadsToDisplay()const noexcept final {
		next_quads_to_display_.clear();
		root_view_group_.PushQuadsTo(next_quads_to_display_);

		if (std::size(next_quads_to_display_) != std::size(current_quads_to_display_)) {
			current_quads_to_display_ = std::move(next_quads_to_display_);
			return &current_quads_to_display_;
		} else
			if (memcmp(next_quads_to_display_.data(), current_quads_to_display_.data(), std::size(current_quads_to_display_) * sizeof(quad))) {
				current_quads_to_display_ = std::move(next_quads_to_display_);
				return &current_quads_to_display_;
			}

		return nullptr;
	}
	template<typename DrawableT, typename...Ts>
	DrawableT create(Ts&&...as)noexcept {
		static_assert(std::is_move_constructible_v<DrawableT>);
		return DrawableT(root_view_group_, std::forward<Ts>(as)...);
	}
private:
	DrawableGroup root_view_group_;
	mutable std::vector<quad> current_quads_to_display_;
	mutable std::vector<quad> next_quads_to_display_;
};

