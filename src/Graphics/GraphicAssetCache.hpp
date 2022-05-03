#pragma once
#include <vector>
#include <glbinding/gl/gl.h>
#include <gsl/gsl>
#include "../utilities.h"
// For this application there is no real need for any type of Asset|Resource Storage|Manager|Cache e.t.c. stuff. 
// But. As a "control unit" for texture and shader, that class exists.
class GraphicAssetCache {
public:
	gl::GLuint* load_textures(gsl::czstring texture_names[], size_t count, const std::array<uint8_t, 4>& background_rgba_color);
	gl::GLuint* load_shader_programs(gsl::czstring shader_names[], size_t count);
	size_t texture_count()const noexcept {
		return std::size(textures_);
	}
	size_t shader_program_count()const noexcept {
		return std::size(shader_programs_);
	}
	gl::GLuint texture(size_t index)noexcept {
		return *textures(index);
	}
	gl::GLuint shader_program(size_t index)noexcept {
		return *shader_programs(index);
	}
	gl::GLuint* textures(size_t index)noexcept {
		return std::data(textures_) + index;
	}
	gl::GLuint* shader_programs(size_t index)noexcept {
		return std::data(shader_programs_) + index;
	}

	void unload_textures(std::span<gl::GLuint> texturest);
	void unload_shader_programs(std::span<gl::GLuint> shader_programs);

	void unload_textures();
	void unload_shader_programs();

	~GraphicAssetCache() {
		unload_textures();
		unload_shader_programs();
	}
private:
	gl::GLuint build_shader_program(gsl::czstring vertex, gsl::czstring fragment)noexcept;
	gl::GLuint create_shader(gsl::czstring src, gl::GLenum shader_type)noexcept;
private:
	std::vector<gl::GLuint> textures_;
	std::vector<gl::GLuint> shader_programs_;
};