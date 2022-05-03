#include <algorithm>
#include "ShaderProgram.hpp"
#include "GraphicAssetCache.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


gl::GLuint* GraphicAssetCache::load_textures(gsl::czstring texture_names[], size_t count, const std::array<uint8_t, 4>& background_rgba_color) {
	textures_.resize(std::size(textures_) + count);
	auto textures_begin = std::data(textures_) + std::size(textures_) - count,
		textures_end = std::data(textures_) + std::size(textures_);

	gl::glGenTextures(count, textures_begin);
	for (auto texture = textures_begin; texture != textures_end; texture = std::next(texture)) {
		auto id = *texture;
		gl::glBindTexture(gl::GL_TEXTURE_2D, id);

		glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_BORDER);//x
		glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_BORDER);//y
		glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
		glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);

		gl::GLfloat borderColor[] = { 1, 1, 1, 0.0f };
		gl::glTexParameterfv(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_BORDER_COLOR, borderColor);

		int w, h, channels;
		stbi_set_flip_vertically_on_load(true);
		const auto filename = *(texture_names + std::distance(textures_begin, texture));
		const auto image = stbi_load(filename, &w, &h, &channels, STBI_rgb_alpha);

		for (auto pixel_component = image; pixel_component != image + (4 * w * h); pixel_component += 4)
		{
			bool make_background_pixel_transparent = true;
			for (uint8_t c = 0; c < 3; ++c)
				if (background_rgba_color[c] != pixel_component[c]) {
					make_background_pixel_transparent = false;
					break;
				}
			if (make_background_pixel_transparent)
				pixel_component[3] = 0;
		}

		gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGBA, w, h, 0, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, image);
		gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
	}
	return std::data(textures_) + std::size(textures_) - count;
}

gl::GLuint* GraphicAssetCache::load_shader_programs(gsl::czstring shader_names[], size_t count) {
	shader_programs_.resize(std::size(shader_programs_) + count);
	for (size_t s = 0; s < count; ++s) {
		ShaderProgramFile shader_program_file(shader_names[s]);
		auto vertex_shader_source = shader_program_file.get_shader_source(ShaderProgramFile::VertexShader);
		auto fragment_shader_source = shader_program_file.get_shader_source(ShaderProgramFile::FragmentShader);

		LOG_INFO("vertex_shader_source \n%s", vertex_shader_source.c_str());
		LOG_INFO("fragment_shader_source \n%s", fragment_shader_source.c_str());

		shader_programs_[std::size(shader_programs_) - count + s] = build_shader_program(vertex_shader_source.c_str(), fragment_shader_source.c_str());
	}
	return std::data(shader_programs_) + std::size(shader_programs_) - count;
}

void GraphicAssetCache::unload_textures(std::span<gl::GLuint> textures) {
	textures_.erase(std::remove_if(std::begin(textures_), std::end(textures_), [textures](gl::GLuint tex){
		return std::find(std::begin(textures), std::end(textures), tex) != std::end(textures);
	}), std::end(textures_));
#if 0
	for (auto texture = std::begin(textures); texture != std::end(textures); texture = std::next(texture))
		std::erase(textures_, *texture);
#endif
	gl::glDeleteTextures(std::size(textures), std::data(textures));
}
void GraphicAssetCache::unload_shader_programs(std::span<gl::GLuint> shader_programs) {

	for (auto program = std::begin(shader_programs); program != std::end(shader_programs); program = std::next(program)) {
		shader_programs_.erase(std::find(std::begin(shader_programs_), std::end(shader_programs_), *program));
#if 0
		std::erase(shader_programs_, *program);
#endif
		gl::glDeleteProgram(*program);
	}
}

void GraphicAssetCache::unload_textures() {
	gl::glDeleteTextures(std::size(textures_), std::data(textures_));
	textures_.clear();
}
void GraphicAssetCache::unload_shader_programs() {
	for (auto program = std::begin(shader_programs_); program != std::end(shader_programs_); program = std::next(program))
		gl::glDeleteProgram(*program);
	shader_programs_.clear();
}
gl::GLuint GraphicAssetCache::build_shader_program(gsl::czstring vertex, gsl::czstring fragment)noexcept {
	using namespace gl;
	auto vertex_shader = create_shader(vertex, GL_VERTEX_SHADER);
	auto fragment_shader = create_shader(fragment, GL_FRAGMENT_SHADER);

	const gl::GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	gl::GLint isLinked = 0;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		GLint logSize = 0;
		// To display error infos alloca is used: there is no need to allocate heap memory for short strings,
		// but error info strings haven't known length, so alloca is used to allocate dynamic lengthed short strings at stack.
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &logSize);
		auto message = (char*)(alloca(logSize));
		glGetProgramInfoLog(shader_program, logSize, &logSize, message);
	}
	glValidateProgram(shader_program);


	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return shader_program;
}
gl::GLuint GraphicAssetCache::create_shader(gsl::czstring src, gl::GLenum shader_type)noexcept {
	using namespace gl;
	auto shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, &src, nullptr);
	glCompileShader(shader_id);

	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == static_cast<GLint>(GL_FALSE)) {
		GLint length;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
		auto info = static_cast<GLchar*>(alloca(length * sizeof(GLchar)));
		GLint written;
		glGetShaderInfoLog(shader_id, length, &written, info);
		LOG_INFO("%s\n", info);
	}
	return shader_id;
}