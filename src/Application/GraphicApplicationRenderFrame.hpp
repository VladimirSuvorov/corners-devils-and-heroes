#pragma once
#include <atomic>
#include <vector>
#include "../Window/RenderWindow.hpp"
#include "../Graphics/GraphicAssetCache.hpp"
#include "../Graphics/QuadBatchMeshRenderer.hpp"
#include "../Views/GameView.hpp"
#include "../utilities.h"

class GraphicApplicationRenderFrame :
	public Frame
{
public:
	GraphicApplicationRenderFrame(GameView& view) :
		view_(&view)
	{}
	void update(std::vector<quad>&& quads);

private:
	void OnInitialize()final;
	void OnDispose()final;
	void OnRender()final;
private:
	std::atomic<GameView*> view_;
	deferred<QuadBatchRenderMesh> quad_batch_render_mesh_;
	std::vector<quad> quads_;
	GraphicAssetCache cache_;
	std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
};

