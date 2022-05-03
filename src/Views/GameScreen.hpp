#pragma once
#include "../Graphics/Drawable.hpp"
#include "../Graphics/QuadBatchMeshRenderer.hpp"

// Several screens to display win, lose e.t.c. game states

template<size_t index>
class GameStateScreen : //Foreground
	public Drawable
{
public:
	GameStateScreen() {
		info_window_.geometry = {
			-(512.f - 256.f) / 512.f,
			1 - (512.f - 128.f) / 512.f,
			2 * 256.f / 512.f,
			2 * 128.f / 512.f
		};
		info_window_.texture_region = {
			0.f,
			1.f - (index * 128.f) / 640.f,
			256.f / 256.f,
			128.f / 640.f
		};
		info_window_.filter_color = { 1, 1, 1, 0.9f };
	}
	void PushQuadsTo(std::vector<quad>& quads)const noexcept final {
		quads.emplace_back(info_window_.to_quad());
	}
	size_t GetQuadCount()const noexcept final {
		return 1;
	}
private:
	sprite info_window_;
};

using TutorialScreen = GameStateScreen<1>;
using WinScreen = GameStateScreen<2>;
using LooseScreen = GameStateScreen<3>;
using NoMoreMovesScreen = GameStateScreen<4>;




