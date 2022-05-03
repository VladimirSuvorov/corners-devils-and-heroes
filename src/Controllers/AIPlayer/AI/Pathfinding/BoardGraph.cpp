#include "BoardGraph.hpp"


void BoardGraph::build() {
	for (uint8_t y = 0; y < height_; ++y)
		for (uint8_t x = 0; x < width_; ++x) {
			auto& node = graph_node(x, y);
			if (std::empty(node.get_children())) {
				node.set_position(pos2d{ int8_t(x), int8_t(y) });
				if (y > 0)
					node.add_child(graph_node(x, y - 1), 1);
				if (y < (width_ - 1))
					node.add_child(graph_node(x, y + 1), 1);
				if (x > 0)
					node.add_child(graph_node(x - 1, y), 1);
				if (x < (height_ - 1))
					node.add_child(graph_node(x + 1, y), 1);
			}
		}
}