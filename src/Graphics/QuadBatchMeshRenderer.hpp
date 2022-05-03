#pragma once
#include <vector>
#include <glbinding/gl/gl.h>
#include "GraphicTypes.hpp"
#include <gsl/gsl>

// All the graphics are a set of sprites. But all the sprites are drawn as a single mesh object.
// So, all the graphics draw proccess is an updating a dynamic mesh of quads (actually - separate triangles - see index buffer),
// that is a single drawing object for all the application (there is nothing to be drawn except that mesh: all the graphics are "baked" into that mesh).
class QuadBatchRenderMesh {
public:
	// First argument is exactly "atlas texture", not a "texture atlas": this is about a single texture that is an atlas.
	QuadBatchRenderMesh(gl::GLuint atlas_texture, gl::GLuint shader_program);

	~QuadBatchRenderMesh();
	
	void Update(std::span<const quad> quads) noexcept {
		if (!std::empty(quads)) {
			update_vertex_buffer(quads);
			update_index_buffer(std::size(quads));
		}
	}

	void Draw() noexcept;

	void set_texture_atlas(gl::GLuint atlas_texture) {
		atlas_texture_ = atlas_texture;
	}
	void set_shader_program(gl::GLuint shader_program) {
		shader_program_ = shader_program;
	}
private:
	void update_vertex_buffer(std::span<const quad> quads) noexcept;
	void update_index_buffer(size_t count) noexcept;
private:
	gl::GLuint atlas_texture_, shader_program_;
	gl::GLint texture_uniform_location_;
private:
	gl::GLuint vertex_array_object_;
	gl::GLuint index_buffer_object_;
	gl::GLsizei indices_to_draw_;

	gl::GLuint vertex_buffer_object_;
	std::size_t max_vertices_;
	std::size_t max_indices_;
private:
	std::vector<gl::GLuint> indices_;
};
