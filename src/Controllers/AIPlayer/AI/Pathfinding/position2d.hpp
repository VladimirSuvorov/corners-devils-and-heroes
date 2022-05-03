#pragma once


struct pos2d {//Represents 2d position for A* node
	int8_t x, y;
	float_t distance_to(const pos2d& target)const {
		const auto v = ((~int16_t(target.x - x)) + (~int16_t(target.y - y)) + 2);// same as: abs(target.x - x) + abs(target.y - y)
		return v * v * v;//this is to make A* algorithm more greedy
	}
};