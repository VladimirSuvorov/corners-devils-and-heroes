#include "GraphicTypes.hpp"
quad sprite::to_quad() const noexcept {
	quad result;

	result[0].position2d = { geometry.left, geometry.top };
	result[0].color = filter_color;
	result[0].texture_coordinates = { texture_region.left, texture_region.top };

	result[1].position2d = { geometry.left + geometry.width, geometry.top };
	result[1].color = filter_color;
	result[1].texture_coordinates = { texture_region.left + texture_region.width, texture_region.top };

	result[2].position2d = { geometry.left, geometry.top - geometry.height };
	result[2].color = filter_color;
	result[2].texture_coordinates = { texture_region.left, texture_region.top - texture_region.height };

	result[3].position2d = { geometry.left + geometry.width,  geometry.top - geometry.height };
	result[3].color = filter_color;
	result[3].texture_coordinates = { texture_region.left + texture_region.width, texture_region.top - texture_region.height };

	return result;
}

