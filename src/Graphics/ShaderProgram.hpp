#pragma once
#include <sstream> // sstream is slow. But it is useful to preprocess shader source code, as far as it used only once per shader load.
#include <fstream>
#include <filesystem>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <glm/glm.hpp>
#include "../utilities.h"


// File that containse shader source code is a single file, having two sections that have to be preprocessed:
// a vertex shader section and a fragment shader section. 
// Any section starts with preamble such as "#shader_type <shader_type>". 
// A shader source code starts from a new line, immediately following by preamble, and ends jsut before a next preamble or end of file.
// To preprocess such file structure are used such slow things as std::stringstream, std::getline è std::ifstream.
// I assume, that for a test task there is no need to use such powerful, but too heavy things such as boost (in instance <boost/iostreams/device/mapped_file.hpp>).
class ShaderProgramFile {
public:
	enum shader_type { VertexShader, FragmentShader, ShaderCount };

	ShaderProgramFile(std::filesystem::path shader_program_filepath) {
		std::ifstream shader_program_file(shader_program_filepath);
		Expects(shader_program_file.is_open());
		parse_shader_program_file(shader_program_file);
	}

	std::string get_shader_source(shader_type shader_type)const {
		return shader_sources_[shader_type].str();
	}
private:
	void parse_shader_program_file(std::ifstream &shader_program_file) {
		std::string string_line;
		while (std::getline(shader_program_file, string_line)) {
			if (string_line.find("#shader_type") != std::string::npos) {
				if (string_line.find("vertex_shader") != std::string::npos)
					current_shader_type_ = VertexShader;
				else
					if (string_line.find("fragment_shader") != std::string::npos)
						current_shader_type_ = FragmentShader;
					else {
						LOG_INFO("such shaders are not supported = %s\n", string_line.c_str());
						Expects(false); // geometry and tesselation shaders are not supported
					}
			}
			else
				if ((string_line.find_first_not_of(" \t\n") != std::string::npos) || current_shader_type_ == VertexShader || current_shader_type_ == FragmentShader)
					shader_sources_[current_shader_type_] << string_line << '\n';
		}
	}
private:
	std::stringstream shader_sources_[ShaderCount];
	shader_type current_shader_type_;
};