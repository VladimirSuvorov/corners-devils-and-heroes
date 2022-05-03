#pragma once
#include <vector>
#include <atomic>
#include <numeric>
#include <algorithm>
#include "GraphicTypes.hpp"

class Drawable {//It could be a scene graph leaf, but in this application is no need in a real scene graph.
public:
	virtual void PushQuadsTo(std::vector<quad>& quads)const = 0;
	virtual size_t GetQuadCount()const = 0;
};

class DrawableGroup ://It could be a scene graph node, but in this application is no need in a real scene graph.
	public Drawable
{
public:
	void attach(Drawable& drawable) {
		if (std::find(std::begin(drawables_), std::end(drawables_), &drawable) == std::end(drawables_))
			drawables_.emplace_back(&drawable);
	}
	void detach(Drawable& drawable) {
		for (auto found = std::find(std::begin(drawables_), std::end(drawables_), &drawable);
			found != std::end(drawables_);
			found = std::find(std::begin(drawables_), std::end(drawables_), &drawable)) {
			auto distance = std::distance(std::begin(drawables_), std::prev(found, 1));
			drawables_.erase(found);
			found = std::next(std::begin(drawables_), distance);
		}
	}
	void PushQuadsTo(std::vector<quad>& quads)const final {
		size_t quads_count = GetQuadCount();
		if (quads.capacity() - std::size(quads) < quads_count)
			quads.reserve(std::size(quads) + quads_count);
		for (auto drawable : drawables_)
			drawable->PushQuadsTo(quads);
	}
	size_t GetQuadCount()const final {
		return std::accumulate(std::begin(drawables_), std::end(drawables_), 0, [](size_t count, Drawable* d) {
			return count + d->GetQuadCount();
			});
	}
private:
	std::vector<Drawable*> drawables_;//set здесь не подойдёт. Или использовать ZBuffer. Или зависеть от порядка отрисовки.
	std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};