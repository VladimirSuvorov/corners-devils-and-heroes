#pragma once
#include <optional>
#include <chrono>
#include <random>
#include "PawnView.hpp"
#include "../Graphics/QuadBatchMeshRenderer.hpp"
#include "../Graphics/Drawable.hpp"
#include "BoardVisualGrid.hpp"
#include "../utilities.h"




// Notice: Animated Pawn View contains animation-related data (mutable).
// Because of all animation data is tightly bound with view data, all these data are placed in this class.
// And because of there is not so much anumation data as well, as this view is "animated" view, there is no real need for a separate class for an animation.
// (Of course, if you're a fan of single-responsibility principle, you're right that this class violates this principle a bit, but not so much to make it matter for test task.)
class AnimatedPawnView :
	public PawnView,
	public Drawable
{
public:
	AnimatedPawnView() = delete;
	AnimatedPawnView(DrawableGroup& relates_to, BoardVisualGrid& grid, AABB texture_region)noexcept :
		relates_to_(&relates_to),
		grid_(grid)
	{
		relates_to_->attach(*this);
		set_texture_region(texture_region);
	}
	AnimatedPawnView(AnimatedPawnView&& src)noexcept :
		relates_to_(src.relates_to_),
		grid_(src.grid_)
	{
		relates_to_->detach(src);
		src.relates_to_ = nullptr;
		memcpy(&current_position_view_, &src.current_position_view_, sizeof(sprite));
		if (src.previous_position_view_)
			previous_position_view_ = std::move(src.previous_position_view_);
		relates_to_->attach(*this);
	}
	AnimatedPawnView(const AnimatedPawnView& oth) = delete;
	const AnimatedPawnView& operator=(const AnimatedPawnView& oth) = delete;

	~AnimatedPawnView()noexcept {
		if (relates_to_)
			relates_to_->detach(*this);
	}
public://PawnView
	void set_pawn_position(board_cell position)noexcept final {
		if (move_update_enabled_)
			current_position_view_ = sprite{ grid_.get_tile(position), filter_color_ * glm::vec4{ 1, 1, 1, 1 }, current_position_view_.texture_region };
	}

	void on_pawn_position_changed(board_cell from, board_cell to)noexcept final {
		if (move_update_enabled_) {
			previous_position_view_ = sprite{ grid_.get_tile(from), filter_color_ * glm::vec4{ 1, 0.7, 0.3, 1 }, current_position_view_.texture_region };
			current_position_view_ = sprite{ grid_.get_tile(to), filter_color_ * glm::vec4{ 0.7, 1, 0.3, 0 }, current_position_view_.texture_region };
		}
	}
	void set_move_update_enabled(bool enabled) {
		move_update_enabled_ = enabled;
	}
public://Drawable
	void PushQuadsTo(std::vector<quad>& quads)const noexcept final {
		// "Play something" are used to compute actual quads - therefore these methods are placed in this one.
		// "PushQuadsTo" is used to get quad data for each drawable object each time it is necessary, and is good for animation update.
		// This method is 'const' because it is logically 'const' and (logically) doesn't change any view data,
		// but animated data is a special case that actually changes each time - even if a 'view' has no chages.
		play_frame_animation();
		play_color_animation();
		if (previous_position_view_)
			quads.emplace_back(previous_position_view_->to_quad());
		quads.emplace_back(current_position_view_.to_quad());
	}
	size_t GetQuadCount()const noexcept final {
		return previous_position_view_ ? 2 : 1;
	}

	void set_texture_region(AABB texture_region)noexcept {
		current_position_view_.texture_region = texture_region;
	}
	void set_filter_color(glm::vec4 filter_color)noexcept {
		filter_color_ = filter_color;
	}

private:
	void play_frame_animation()const noexcept {
		std::chrono::duration<float_t> elapsed = std::chrono::high_resolution_clock::now() - frame_animation_loop_start_;
		constexpr uint8_t AnimationFrameCount = 3;

		constexpr std::chrono::duration<float_t> FrameAnimationLoopDuration = 0.5f * AnimationFrameCount * 1s;//3 frames * 1s duration

		while (elapsed > FrameAnimationLoopDuration)
			elapsed -= FrameAnimationLoopDuration;
		
		frame_animation_loop_start_ = std::chrono::high_resolution_clock::now() - std::chrono::duration_cast<decltype(frame_animation_loop_start_)::duration>(elapsed);

		if (elapsed <= FrameAnimationLoopDuration) {
			float_t approach = (elapsed / FrameAnimationLoopDuration);
			uint8_t frame_to_display = AnimationFrameCount * approach;// *approach* approach;
			if (frame_to_display != current_animation_frame_) {
				if (current_animation_frame_ < frame_to_display)
					for (uint8_t frame_move = 0; frame_move < (frame_to_display - current_animation_frame_); ++frame_move) {
						current_position_view_.texture_region = current_position_view_.texture_region.move_right();
						if (previous_position_view_)
							previous_position_view_->texture_region = previous_position_view_->texture_region.move_right();
					}
				else
					for (uint8_t frame_move = 0; frame_move < (current_animation_frame_ - frame_to_display); ++frame_move) {
						current_position_view_.texture_region = current_position_view_.texture_region.move_left();
						if (previous_position_view_)
							previous_position_view_->texture_region = previous_position_view_->texture_region.move_left();
					}
				current_animation_frame_ = frame_to_display;
			}
		}
	}
	void play_color_animation()const noexcept {
		
		if (previous_position_view_) {

			auto approach = [this]()->float_t {
				if (color_animation_start_) [[likely]] {
					auto elapsed = std::chrono::high_resolution_clock::now() - *color_animation_start_;
					using namespace std::chrono_literals;
					constexpr std::chrono::duration<float_t> Duration = 1s;
					if (elapsed <= Duration)
						return elapsed / Duration;
					else {
						color_animation_start_.reset();
						previous_position_view_.reset();
						current_position_view_ = sprite{ current_position_view_.geometry, filter_color_ * glm::vec4{ 1, 1, 1, 1 }, current_position_view_.texture_region };
					}
				} else 
					color_animation_start_.emplace(std::chrono::high_resolution_clock::now());
				return 0.f;
			}();
			if (previous_position_view_) {
				previous_position_view_->filter_color = filter_color_ * glm::vec4{ 1.f * (approach * approach), 0.7f - (0.7f * approach), 0.3f - (0.3f * approach), 1.f - (approach * approach)};
				current_position_view_.filter_color = filter_color_ * glm::vec4{ 0.7f * approach, 1.f * (approach * approach), 0.3f * approach, (approach * approach) };
			}
		}else
			current_position_view_ = sprite{ current_position_view_.geometry, filter_color_ * glm::vec4{ 1, 1, 1, 1 }, current_position_view_.texture_region };
	}


	static int64_t get_random_animation_start_miliseconds_bias() {
		// To make view meaningful, each object's view is individual, so - to achieve that individuality of visible objects, all animations have a time bias, that is computed with this method.
		static std::random_device random_device;
		static std::mt19937_64 engine(std::seed_seq({ random_device(), random_device(), random_device() }));
		static std::uniform_real_distribution<float_t> distribution(1.f, 10000.f);
		return distribution(engine);
	}
private:
	DrawableGroup* relates_to_;
	bool move_update_enabled_ = true;
	glm::vec4 filter_color_ = { 1, 1, 1, 1 };
private:
	mutable sprite current_position_view_;
	mutable std::optional<sprite> previous_position_view_;
private:
	mutable std::optional<std::chrono::high_resolution_clock::time_point> color_animation_start_;

	mutable uint8_t current_animation_frame_ = 0;
	mutable std::chrono::high_resolution_clock::time_point frame_animation_loop_start_ = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds(get_random_animation_start_miliseconds_bias());
	BoardVisualGrid& grid_;
};
