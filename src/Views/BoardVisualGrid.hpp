#pragma once
#include "../Graphics/GraphicTypes.hpp"
#include "../Models/GameRules.hpp"
// Just an interface with a single method, converting board_cell position to AABBox with coordinates in OpenGL screen space (range [-1; 1])
class BoardVisualGrid {
public:
	virtual AABB get_tile(board_cell cell)const = 0;
};