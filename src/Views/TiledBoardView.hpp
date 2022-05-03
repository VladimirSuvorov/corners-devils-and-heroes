#pragma once
#include "../Graphics/Drawable.hpp"
// This view class is used to dispaly a game board (a.k.a. checkerboard).
// The method is used to display the board is 8*8 tile sprite displaying.
// All the stuff is going in OpenGL screen space with range [-1; 1].
class TiledBoardView : //Background
	public Drawable
{
public:
	TiledBoardView() {
		for (uint8_t y = 0; y < 8; ++y)
			for (uint8_t x = 0; x < 8; ++x) {
				auto& tile = tiles_[y * 8 + x];
				tile.geometry = {
					-1 + 2 * x * 64.f / 512.f,
					1.f - 2 * y * 64.f / 512.f,
					2 * 64.f / 512.f,
					2 * 64.f / 512.f
				};
				tile.texture_region = {
					1.f - 0.25f,
					1.f - ((x + y) % 2) * 0.1f,
					0.25f,
					0.1f
				};
				tile.filter_color = { 1, 1, 1, 1 };
			}
	}

	void PushQuadsTo(std::vector<quad>& quads)const noexcept final {
		for (auto &tile : tiles_)
			quads.emplace_back(tile.to_quad());
	}
	size_t GetQuadCount()const noexcept final {
		return std::size(tiles_);
	}
private:
	std::array<sprite, 8 * 8> tiles_;
};
