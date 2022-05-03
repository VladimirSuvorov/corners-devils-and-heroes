#include "Pathfinder.hpp"

template<typename Position>
inline bool AStar<Position>::get_path(AStarNode<Position>* start, AStarNode<Position>* goal, std::vector<AStarNode<Position>*>& path, std::vector<AStarNode<Position>*>* incomplete_path) {
	//printf("get_path\n");
	using node_t = AStar<Position>::node_type;
	using position_t = node_t::position_type;
	//clear();
	std::make_heap(std::begin(open_), std::end(open_), node_comparator<position_t>());
	push_open(start);

	node_t* closest_node = nullptr;
	while (!std::empty(open_)) {
		std::sort(std::begin(open_), std::end(open_), node_comparator<position_t>());
		auto current_node = open_.front(); // pop node from open_ for which f is minimal
		pop_open(current_node);

		current_node->set_closed(true);
		Expects(current_node);
		closed_.emplace_back(current_node);

		if (current_node == goal) {
			build_path(current_node, path);
			return true;
		}
#if 0
		for (const auto& child : current_node->get_children()) { // for each successor n' of n
#endif
		auto& children = current_node->get_children();
		for (auto cit = children.begin();
			cit != children.end();
			++ cit) {
			auto child = *cit;

			auto [child_node, child_node_weight] = child;//child is Null struct
			auto astar_child_node = child_node;
			float_t g = current_node->get_g() + child_node_weight;

			if ((astar_child_node->is_open() || astar_child_node->is_closed()) && astar_child_node->get_g() < g) // node is already in opend or closed with a lower cost g(node)
				continue;

			float_t h = distance_between(astar_child_node, goal);
			float_t f = g + h;
			astar_child_node->set_parent(current_node);
			astar_child_node->set_f(f);
			astar_child_node->set_g(g);
			astar_child_node->set_h(h);
#if 0
			if (!closest_node || (astar_child_node->get_h() < closest_node->get_h()))
				closest_node = astar_child_node;
#endif

			if (astar_child_node->is_closed())
				astar_child_node->set_closed(false);
			if (!astar_child_node->is_open())
				push_open(astar_child_node);

			if (!closest_node || (astar_child_node->get_h() < closest_node->get_h()))
				closest_node = astar_child_node;
		}
	}
	if (closest_node && incomplete_path)
		build_path(closest_node, *incomplete_path);

	return false;
}
