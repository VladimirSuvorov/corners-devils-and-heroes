#pragma once
#include <inttypes.h>
#include "../../../../Models/GameRules.hpp"
#include "Pathfinder.hpp"
#include "position2d.hpp"

class BoardGraph {//Builds & contains nodes (a graph) for A* algorithm, for 2d board of any given size (just need to replace board_graph_t with std::vector instead of std::array).
public:
	//These aliases are to simplify type compatibility.
	using algorithm_t = AStar<pos2d>;
	using node_t = typename algorithm_t::node_type;
	using board_graph_t = std::array<node_t, 8 * 8>;//For optimization purposes (for a given game application). Can be easily replaced with std::vector.
	using pathfinder_t = PathFinder<node_t>;
	using path_t = std::vector<node_t*>;

	BoardGraph(uint32_t width, uint32_t height, std::span<board_cell> obstacles = {}) :
		width_(width),
		height_(height)
	{
		Expects(width_ && height_ && std::size(nodes_));
		build();
		reset_obstacles(obstacles);
	}
	void clear_closed() {
		Expects(width_ && height_ && std::size(nodes_));
		for (auto& node : nodes_)
			node.set_closed(false);
	}
	void set_closed_nodes(std::span<board_cell> positions, bool closed) {
		Expects(width_ && height_ && std::size(nodes_));
		for (auto p : positions)
			graph_node(p.x(), p.y()).set_closed(closed);
	}
	void reset_obstacles(std::span<board_cell> obstacles) {
		Expects(width_ && height_ && std::size(nodes_));
		clear_closed();
		set_closed_nodes(obstacles, true);
	}
	std::span<node_t> nodes() {
		return nodes_;
	}
	node_t& graph_node(uint32_t x, uint32_t y) {
		Expects(width_ && height_ && std::size(nodes_));
		return nodes_[y * width_ + x];
	}
	const node_t& graph_node(uint32_t x, uint32_t y)const {
		return const_cast<BoardGraph*>(this)->graph_node(x, y);
	}
private:
	void build();
private:
	uint32_t width_ = 0, height_ = 0;
	board_graph_t nodes_;
};