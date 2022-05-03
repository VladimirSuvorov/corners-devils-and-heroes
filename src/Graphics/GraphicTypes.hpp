#pragma once
#include <array>
#include <glm/glm.hpp>
#include <float.h>
#include "../utilities.h"
// This file contains very basic types related to graphics. 
// Each value follows OpenGL convention: coordinates are in screen space range [-1;1], color components are in range [0;1]

struct vertex {
	glm::vec2 position2d;
	glm::vec4 color;
	glm::vec2 texture_coordinates;
};


using quad = std::array<vertex, 4>;

struct AABB {//AABB: A - axis A - aligned B - bounding B - box
	float_t left, top, width, height;
	AABB move_left() {
		return { left - width, top, width, height };
	}
	AABB move_right() {
		return { left + width, top, width, height };
	}
	AABB move_up() {
		return { left, top + height, width, height };
	}
	AABB move_down() {
		return { left, top - height, width, height };
	}
};


struct sprite {
	AABB geometry = { 0,0,0,0 };
	AABB texture_region = { 0,0,0,0 };
	glm::vec4 filter_color = { 0,0,0,0 };

	sprite(AABB geometry = {}, glm::vec4 filter_color = glm::vec4(), AABB texture_region = {})noexcept {
		reset(geometry, filter_color, texture_region);
	}

	sprite& reset(AABB geometry, glm::vec4 filter_color, AABB texture_region = {})noexcept {
		this->geometry = geometry;
		this->filter_color = filter_color;
		this->texture_region = texture_region;
		return *this;
	}

	quad to_quad() const noexcept;
};

