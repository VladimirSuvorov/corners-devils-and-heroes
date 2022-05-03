#include <numeric>
#include "AlternativeSelector.hpp"

goal_info::goal_info(board_cell g, std::vector<node_t*>& path) :
	goal(g)
{
	path_to_the_goal.resize(std::size(path));
	std::transform(std::begin(path), std::end(path), std::begin(path_to_the_goal), [](const node_t* n)->board_cell {
		return board_cell(n->position().x, n->position().y);
	});

#if 0
	std::array<char, 9 * 8 > path_view;
	memset(std::data(path_view), '#', std::size(path_view) * sizeof(typename decltype(path_view)::value_type));
	for (uint8_t row = 0; row < 8; ++row)
		path_view[(9 * row) + 8] = '\0';
	for (auto n : path_to_the_goal)
		path_view[(uint16_t(n.y()) * 9) + uint16_t(n.x())] = '*';
	printf("========\n");
	for (uint8_t row = 0; row < 8; ++row)
		printf("%s\n", std::data(path_view) + (9 * row));
	printf("========\n");
#endif
}

movable_pawn_info::movable_pawn_info(AIPawnProxy& p) {
	pawn = &p;
	min_approachable_distance = std::numeric_limits<decltype(min_approachable_distance)>::max();
	max_approachable_distance = std::numeric_limits<decltype(max_approachable_distance)>::min();
	min_reachable_distance = std::numeric_limits<decltype(min_reachable_distance)>::max();
	max_reachable_distance = std::numeric_limits<decltype(max_reachable_distance)>::min();
}

movable_pawn_info::movable_pawn_info() {
	pawn = nullptr;
	min_approachable_distance = std::numeric_limits<decltype(min_approachable_distance)>::max();
	max_approachable_distance = std::numeric_limits<decltype(max_approachable_distance)>::min();
	min_reachable_distance = std::numeric_limits<decltype(min_reachable_distance)>::max();
	max_reachable_distance = std::numeric_limits<decltype(max_reachable_distance)>::min();
}

template<typename InIt, typename OutIt, typename Pred, typename Fn>
void transform_if(InIt src_begin, InIt src_end, OutIt dst_begin, Pred cond, Fn func) {
	for (auto it = src_begin; it != src_end; it = std::next(it))
		if (cond(*it)) {
			*dst_begin = func(*it);
			++dst_begin;
		}
}

void movable_pawn_info::push_alternatives(std::vector<alternative_unique_ptr_t>& alternatives, AlternativesFactory& factory)const {
	// Before this method we were interested in things such as goals, pathes e.t.c., to prioritize alternatives.
	// But, since we've entered this method, we don't care about this anymore.
	// All we want here is just to select a set of cells on the game board that player's pawns could visit. That's it: neither more, nor less.
	// So, we collapsing all pathes to the next (after a pawn stands in) node - as well as collapsing all pathes, having such node same between each other.

	std::vector<board_cell> next_nodes;
	next_nodes.reserve(std::size(reachable_goals) + std::size(approachable_goals));
	transform_if(std::begin(reachable_goals), std::end(reachable_goals), std::back_inserter(next_nodes), [&next_nodes](const goal_info& gp)->bool {
		return (std::find(std::begin(next_nodes), std::end(next_nodes), *std::begin(gp.path_to_the_goal)) == std::end(next_nodes));
	}, [&next_nodes](const goal_info& gp)->board_cell {
		return *std::begin(gp.path_to_the_goal);//*std::next(std::begin(gp.path_to_the_goal));
	});
	transform_if(std::begin(approachable_goals), std::end(approachable_goals), std::back_inserter(next_nodes), [&next_nodes](const goal_info& gp)->bool {
		return (std::find(std::begin(next_nodes), std::end(next_nodes), *std::begin(gp.path_to_the_goal)) == std::end(next_nodes));
	}, [&next_nodes](const goal_info& gp)->board_cell {
		return *std::begin(gp.path_to_the_goal);
	});	
	for (auto next_node : next_nodes)
		alternatives.emplace_back(const_cast<movable_pawn_info*>(this)->pawn->get_alternative(factory, next_node));
}

std::vector<movable_pawn_info> CollectInfoAboutMovablePawns(std::span<AIPawnProxy> pawns, std::span<board_cell> goals, std::span<board_cell> obstacles) {
	std::vector<movable_pawn_info> infos;
	const size_t infos_required = std::accumulate(std::begin(pawns), std::end(pawns), 0, [obstacles](size_t count, const AIPawnProxy& p)->size_t {
		return count + p.can_move_left() + p.can_move_up() + p.can_move_right() + p.can_move_down();
	});
	infos.reserve(infos_required);

	using node_t = BoardGraph::node_t;
	using algorithm_t = BoardGraph::algorithm_t;
	using pathfinder_t = BoardGraph::pathfinder_t;
	using path_t = BoardGraph::path_t;
	BoardGraph board_graph(8, 8, obstacles);
	pathfinder_t pathfinder;
	path_t reachable_path, unreachable_path;
	algorithm_t algorithm;

	// Test all possible cells pawn can move to (at the moment),
	// collect info about pathes to goals for each

	auto AddInfo = [&](AIPawnProxy& pawn) {
		auto& info = infos.emplace_back(pawn);
		for (auto goal : goals) {
			board_graph.reset_obstacles(obstacles);
			pathfinder.set_destination(board_graph.graph_node(goal.x(), goal.y()));
			if (pathfinder.find_path<algorithm_t>(algorithm, reachable_path, &unreachable_path))
				if (!std::empty(reachable_path))
					info.on_goal_reachable(goal, reachable_path, pawn);
				else info.has_already_reached_goal = true;
			else
				if (!std::empty(unreachable_path))
					info.on_goal_approachable(goal, unreachable_path, pawn);
			reachable_path.clear();
			unreachable_path.clear();
			algorithm.clear();
		}
		Ensures(info.possible_alternative_count());
	};

	for (auto& pawn : pawns) {
		if (!pawn.movable())
			continue;

		if (pawn.can_move_right()) {
			const auto from = pawn.desired_cell.get_right_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn);
		}
		if (pawn.can_move_down()) {
			const auto from = pawn.desired_cell.get_bottom_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn);
		}
		if (pawn.can_move_left()) {
			const auto from = pawn.desired_cell.get_left_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn);
		}
		if (pawn.can_move_up()) {
			const auto from = pawn.desired_cell.get_top_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn);
		}
	}
	Ensures(!(std::empty(infos) && infos_required));
	return infos;
}

std::vector<movable_pawn_info> MultithreadedCollectInfoAboutMovablePawns(std::span<AIPawnProxy> pawns, std::span<board_cell> goals, std::span<board_cell> obstacles) {
	std::vector<movable_pawn_info> infos;
	std::atomic_flag infos_lock = ATOMIC_FLAG_INIT;
	const size_t infos_required = std::accumulate(std::begin(pawns), std::end(pawns), 0, [obstacles](size_t count, const AIPawnProxy& p)->size_t {
		return count + p.can_move_left() + p.can_move_up() + p.can_move_right() + p.can_move_down();
	});
	constexpr size_t TotalThreadCount = MaxCollectInfoThreadCount + 1;
	const size_t part_size = std::size(pawns) / TotalThreadCount;
	if (!part_size)//if no enough work for multiple threads recall single-threaded version
		return CollectInfoAboutMovablePawns(pawns, goals, obstacles);

	infos.reserve(infos_required);

	using node_t = BoardGraph::node_t;
	using algorithm_t = BoardGraph::algorithm_t;
	using pathfinder_t = BoardGraph::pathfinder_t;
	using path_t = BoardGraph::path_t;
	auto AddInfo = [&](AIPawnProxy& pawn, BoardGraph& board_graph, pathfinder_t& pathfinder, algorithm_t& algorithm,
		path_t &reachable_path, path_t& unreachable_path) {
		while (infos_lock.test_and_set(std::memory_order_relaxed));//lock info creation, that is not multithreaded
		Expects(std::size(infos) < infos.capacity());
		auto& info = infos.emplace_back(pawn);
		infos_lock.clear(std::memory_order_relaxed);
		for (auto goal : goals) {
			board_graph.reset_obstacles(obstacles);
			pathfinder.set_destination(board_graph.graph_node(goal.x(), goal.y()));
			if (pathfinder.find_path<algorithm_t>(algorithm, reachable_path, &unreachable_path))
				if (!std::empty(reachable_path))
					info.on_goal_reachable(goal, reachable_path, pawn);
				else info.has_already_reached_goal = true;
			else
				if (!std::empty(unreachable_path))
					info.on_goal_approachable(goal, unreachable_path, pawn);
			reachable_path.clear();
			unreachable_path.clear();
			algorithm.clear();
		}
		Ensures(info.possible_alternative_count());
	};

	using thread_index_t = size_t;
	std::array<std::thread, MaxCollectInfoThreadCount> therads;

	for (thread_index_t thread_index = 0; thread_index < MaxCollectInfoThreadCount; ++thread_index)
		therads[thread_index] = std::thread([&infos, &AddInfo, thread_index, part_size, pawns, obstacles, goals]()mutable {
			BoardGraph board_graph(8, 8, obstacles);
			pathfinder_t pathfinder;
			path_t reachable_path, unreachable_path;
			algorithm_t algorithm;
			for (auto p = (thread_index * part_size); p < ((thread_index * part_size) + part_size); ++p) {
				auto& pawn = pawns[p];
				if (!pawn.movable())
					continue;
				if (pawn.can_move_right()) {
					const auto from = pawn.desired_cell.get_right_neighbour();
					pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
					AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
				}
				if (pawn.can_move_down()) {
					const auto from = pawn.desired_cell.get_bottom_neighbour();
					pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
					AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
				}
				if (pawn.can_move_left()) {
					const auto from = pawn.desired_cell.get_left_neighbour();
					pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
					AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
				}
				if (pawn.can_move_up()) {
					const auto from = pawn.desired_cell.get_top_neighbour();
					pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
					AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
				}
			}
		});

	BoardGraph board_graph(8, 8, obstacles);
	pathfinder_t pathfinder;
	path_t reachable_path, unreachable_path;
	algorithm_t algorithm;
	for (auto p = (MaxCollectInfoThreadCount * part_size); p < std::size(pawns); ++p) {
		auto& pawn = pawns[p];
		if (!pawn.movable())
			continue;
		if (pawn.can_move_right()) {
			const auto from = pawn.desired_cell.get_right_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
		}
		if (pawn.can_move_down()) {
			const auto from = pawn.desired_cell.get_bottom_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
		}
		if (pawn.can_move_left()) {
			const auto from = pawn.desired_cell.get_left_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
		}
		if (pawn.can_move_up()) {
			const auto from = pawn.desired_cell.get_top_neighbour();
			pathfinder.set_origin(board_graph.graph_node(from.x(), from.y()));
			AddInfo(pawn, board_graph, pathfinder, algorithm, reachable_path, unreachable_path);
		}
	}
	for (auto& thread : therads)
		thread.join();

	Ensures(!(std::empty(infos) && infos_required));
	Ensures(std::accumulate(std::begin(infos), std::end(infos), 0, [](size_t count, const movable_pawn_info& info)->size_t {
		return count + info.possible_alternative_count();
	}));

	return infos;
}





void PrioritizeMovablePawnInfos(std::vector<movable_pawn_info>& infos) {
	//minify path, maximize distance to goal
	for (auto& info : infos) {
		std::sort(std::begin(info.reachable_goals), std::end(info.reachable_goals), [](const goal_info& l, const goal_info& r)->bool {
			return std::size(l.path_to_the_goal) < std::size(r.path_to_the_goal);
		});
		std::sort(std::begin(info.approachable_goals), std::end(info.approachable_goals), [](const goal_info& l, const goal_info& r)->bool {
			return std::size(l.path_to_the_goal) < std::size(r.path_to_the_goal);
		});
	}

	std::sort(std::begin(infos), std::end(infos), [](const movable_pawn_info& l, const movable_pawn_info& r)->bool {
		if (l.has_already_reached_goal != r.has_already_reached_goal)
			return (l.has_already_reached_goal < r.has_already_reached_goal);		//rule 0: alternatives that have'nt reached goal BEFORE alternatives that have

		if (std::empty(l.reachable_goals) != std::empty(r.reachable_goals))
			return (std::empty(l.reachable_goals) < std::empty(r.reachable_goals));	//rule 1: alternatives that have reachABLE goals BEFORE alternatives that haven't

		if (std::size(l.reachable_goals) != std::size(r.reachable_goals))
			return (std::size(l.reachable_goals) < std::size(r.reachable_goals));	//rule 2: alternatives that have LESS reachABLE goals BEFORE alternatives that have MORE

		if (int16_t(l.min_reachable_distance) != int16_t(r.min_reachable_distance))
			return (l.min_reachable_distance < r.min_reachable_distance);
		
		if (int16_t(l.max_reachable_distance) != int16_t(r.max_reachable_distance))
			return (l.max_reachable_distance < r.max_reachable_distance);

		if (std::size(l.approachable_goals) != std::size(r.approachable_goals))
			return (std::size(l.approachable_goals) < std::size(r.approachable_goals));	//rule N: alternatives that have LESS approachABLE goals BEFORE alternatives that have MORE

		if (int16_t(l.max_approachable_distance) != int16_t(r.max_approachable_distance))
			return (l.max_approachable_distance < r.max_approachable_distance);

		if (int16_t(l.min_approachable_distance) != int16_t(r.min_approachable_distance))
			return (l.min_approachable_distance < r.min_approachable_distance);

		return false;
	});
}

std::vector<alternative_unique_ptr_t> ConvertMovabalePawnInfosToAlternatives(const std::vector<movable_pawn_info>& infos, AlternativesFactory& factory) {
	Ensures(!std::empty(infos));
	const size_t alternative_count = std::accumulate(std::begin(infos), std::end(infos), 0, [](size_t count, const movable_pawn_info& info)->size_t {
		return count + info.possible_alternative_count();
	});
	Expects(alternative_count);
	std::vector<alternative_unique_ptr_t> alternatives;
	alternatives.reserve(alternative_count);
	for (auto& info : infos)
		info.push_alternatives(alternatives, factory);

	for (auto& info : infos) // this is done in this separate for loop, to push "no way" alternatives always at back of vector; no-way alternatives are worst choise that used if no more moves.
		info.push_no_way_alternatives(alternatives, factory);

	Expects(!std::empty(alternatives));
	return alternatives;
}

/////////// helper functions

std::vector<board_cell> GetBoardObstacles(std::span<AIPawnProxy> enemy, std::span<AIPawnProxy> my, Pawn* self) {
	std::vector<board_cell> obstacles;
	if (self) {
		obstacles.reserve(std::size(enemy) + std::size(my) - 1);
		for (auto& enemy_pawn : enemy)
			if (enemy_pawn.pawn != self)
				obstacles.emplace_back(enemy_pawn.desired_cell);
		for (auto& my_pawn : my)
			if (my_pawn.pawn != self)
				obstacles.emplace_back(my_pawn.desired_cell);
	} else {
		obstacles.resize(std::size(enemy) + std::size(my));
		std::transform(std::begin(enemy), std::end(enemy), std::begin(obstacles), [](const AIPawnProxy& p)->board_cell {
			return p.desired_cell;
		});
		std::transform(std::begin(my), std::end(my), std::next(std::begin(obstacles), std::size(enemy)), [](const AIPawnProxy& p)->board_cell {
			return p.desired_cell;
		});
	}
	return obstacles;
}

std::vector<AIPawnProxy*> SelectMovablePawns(std::span<AIPawnProxy> pawns, std::span<board_cell> obstacles) {
	std::vector<AIPawnProxy*> movable;
	movable.reserve(std::size(pawns));
	for (auto& pawn : pawns)
		if (pawn.movable())
			movable.emplace_back(&pawn);
	return movable;
}
