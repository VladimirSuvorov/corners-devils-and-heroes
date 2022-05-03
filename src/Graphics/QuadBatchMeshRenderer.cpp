#include <chrono>
#include <vector>
#include <array>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include "QuadBatchMeshRenderer.hpp"
#include "../utilities.h"
#include "../undef_gl_macros.h"


template<typename V>
constexpr gl::GLint vec_size(V vec_field) {
	return sizeof(V) / sizeof(typename V::value_type);
}


QuadBatchRenderMesh::QuadBatchRenderMesh(gl::GLuint atlas_texture, gl::GLuint shader_program) {
		set_texture_atlas(atlas_texture);
		set_shader_program(shader_program);
		gl::glGenVertexArrays(1, &vertex_array_object_);
		gl::glBindVertexArray(vertex_array_object_);

		gl::glGenBuffers(1, &vertex_buffer_object_);
		gl::glBindBuffer(gl::GL_ARRAY_BUFFER, vertex_buffer_object_);

#define VEC_SIZE_IMPL(Struct, StructVectorField) \
	(sizeof(Struct::StructVectorField) / sizeof(typename decltype(Struct::StructVectorField)::value_type))
#define VEC_SIZE(Struct, StructVectorField) VEC_SIZE_IMPL(Struct, StructVectorField)
#define VERTEX_ATTRIB_POINTER(AttribIndex, VertexType, Attrib, AttribGLType, AttribNormalized)\
	gl::glVertexAttribPointer(AttribIndex, VEC_SIZE(VertexType, Attrib), gl::AttribGLType, AttribNormalized, sizeof(VertexType), (const void*)offsetof(VertexType, Attrib));

		gl::glEnableVertexAttribArray(0);
		VERTEX_ATTRIB_POINTER(0, vertex, position2d, GL_FLOAT, false);

		gl::glEnableVertexAttribArray(1);
		VERTEX_ATTRIB_POINTER(1, vertex, color, GL_FLOAT, false);

		gl::glEnableVertexAttribArray(2);
		VERTEX_ATTRIB_POINTER(2, vertex, texture_coordinates, GL_FLOAT, false);

#undef VEC_SIZE_IMPL
#undef VEC_SIZE
#undef VERTEX_ATTRIB_POINTER

		gl::glGenBuffers(1, &index_buffer_object_);
		gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);

		max_vertices_ = max_indices_ = indices_to_draw_ = 0;

		constexpr auto UniformName = "texture_sampler";
		if ((texture_uniform_location_ = gl::glGetUniformLocation(shader_program, UniformName)) == -1)
			LOG_INFO("Error: location %s not found\n", UniformName);
	}

QuadBatchRenderMesh::~QuadBatchRenderMesh() noexcept {
	gl::glDeleteBuffers(1, &index_buffer_object_);
	gl::glDeleteBuffers(1, &vertex_buffer_object_);
	gl::glDeleteVertexArrays(1, &vertex_array_object_);
}



void QuadBatchRenderMesh::Draw() noexcept {
	gl::glUseProgram(shader_program_);
	//gl::glActiveTexture(gl::GL_TEXTURE0);
	gl::glBindTexture(gl::GL_TEXTURE_2D, atlas_texture_);
	gl::glUniform1i(texture_uniform_location_, 0);
	gl::glBindVertexArray(vertex_array_object_);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);

	gl::glDrawElements(gl::GL_TRIANGLES, indices_to_draw_, gl::GL_UNSIGNED_INT, nullptr);
}


// Bellow you could find a code that looks like a bit of "hard porn": transferring a huge amount of vertices to a video-card for each frame is a definitely bad idea (in fact: horrible idea).
// But. In this application there is no "huge amount of vertices" - in opposite - there not so much vertices and - the most important thing is that the update of mesh
// happens not so often and frequently as "each frame": in opposite, graphics data is sent to GPU only once per several milliseconds. So that is not horrible at all.
void QuadBatchRenderMesh::update_vertex_buffer(std::span<const quad> quads) noexcept {
	const size_t total_vertex_count = std::size(quads) * 4;

	if (total_vertex_count < max_vertices_)
		gl::glBufferSubData(gl::GL_ARRAY_BUFFER, 0, total_vertex_count * sizeof(vertex), std::data(quads));
	else {
		gl::glBufferData(gl::GL_ARRAY_BUFFER, total_vertex_count * sizeof(vertex), std::data(quads), gl::GL_STREAM_DRAW);
		max_vertices_ = total_vertex_count;
	}
}
void QuadBatchRenderMesh::update_index_buffer(size_t count) noexcept {
	using gl::GLuint;
	constexpr GLuint index_pattern[] = {
		0, 1, 2,
		1, 3, 2
	};
	constexpr GLuint IndexPatternSize = std::size(index_pattern);
	indices_.resize(count * IndexPatternSize);
	for (GLuint quad_index = 0; quad_index < count; ++quad_index) {
		const GLuint base_quad_index = quad_index * IndexPatternSize;

		for (GLuint pattern_index = 0; pattern_index < IndexPatternSize; ++pattern_index)
			indices_[base_quad_index + pattern_index] = quad_index * 4 + index_pattern[pattern_index];
	}

	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
	if (std::size(indices_) < max_indices_)
		gl::glBufferSubData(gl::GL_ELEMENT_ARRAY_BUFFER, 0, indices_.size() * sizeof(decltype(indices_)::value_type), indices_.data());
	else {
		gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(decltype(indices_)::value_type), indices_.data(), gl::GL_STREAM_DRAW);
		max_indices_ = indices_.size();
	}
	indices_to_draw_ = indices_.size();
}