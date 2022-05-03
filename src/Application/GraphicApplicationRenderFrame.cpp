#include "GraphicApplicationRenderFrame.hpp"

void GraphicApplicationRenderFrame::update(std::vector<quad>&& quads) {
	while (lock_.test_and_set(std::memory_order_acquire));
	quads_ = std::move(quads);
	lock_.clear(std::memory_order_release);
}

void GraphicApplicationRenderFrame::OnInitialize() {
	static const char* TextureNames[] = { "corners.png" };//TODO: remove hardcode
	static const char* ShaderNames[] = { "DefaultShaderProgram.shader" };
	quad_batch_render_mesh_.create(
		*cache_.load_textures(TextureNames, std::size(TextureNames), { 174, 118, 255, 0 }),
		*cache_.load_shader_programs(ShaderNames, std::size(ShaderNames))
	);
}
void GraphicApplicationRenderFrame::OnDispose() {
	quad_batch_render_mesh_.destroy();
}
void GraphicApplicationRenderFrame::OnRender() {
	gl::glClearColor(0.5, 0.5, 0.5, 1);
	gl::glClear(gl::GL_COLOR_BUFFER_BIT);
	if (auto view = view_.load(std::memory_order_acquire))
		if (auto quads = view->GetQuadsToDisplay()) {
			quad_batch_render_mesh_->Update(*quads);

		}
#if 0
	if (!lock_.test_and_set(std::memory_order_acquire))
		if (!std::empty(quads_)) {
			quad_batch_render_mesh_->Update(quads_);
			quads_.clear();
		}
#endif
	lock_.clear(std::memory_order_release);
	quad_batch_render_mesh_->Draw();
}