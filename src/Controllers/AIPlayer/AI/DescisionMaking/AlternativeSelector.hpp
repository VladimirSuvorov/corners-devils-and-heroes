#pragma once
#include <vector>
#include "../../../../Models/GameRules.hpp"
#include "../Pathfinding/BoardGraph.hpp"
#include "../AIPawnProxy.hpp"
#include "../AIPawnActionAlternative.hpp"


inline float_t Distance(board_cell from, board_cell to) {//TODO: SqrDistance
	return (~(int16_t(to.x()) - int16_t(from.x())) + (~(int16_t(to.y()) - int16_t(from.y())) + 2));
	//return abs(float_t(to.x()) - float_t(from.x())) + abs(float_t(to.y()) - float_t(from.y()));
}
std::vector<AIPawnProxy*> SelectMovablePawns(std::span<AIPawnProxy> pawns, std::span<board_cell> obstacles);


std::vector<board_cell> GetBoardObstacles(std::span<AIPawnProxy> enemy, std::span<AIPawnProxy> my, Pawn* self = nullptr);

struct goal_info {
	board_cell goal;
	std::vector<board_cell> path_to_the_goal;
	using node_t = BoardGraph::node_t;

	goal_info(board_cell goal, std::vector<node_t*>& path);
	goal_info(goal_info&& src) :
		goal(src.goal),
		path_to_the_goal(std::move(src.path_to_the_goal))
	{}
	goal_info& operator=(goal_info&& src) {
		goal = src.goal;
		path_to_the_goal = std::move(src.path_to_the_goal);
		return *this;
	}
	goal_info(const goal_info& oth) = delete;
	goal_info& operator=(const goal_info& oth) = delete;


	float_t distnace_to_the_goal(board_cell from)const noexcept {
		return Distance(from, goal);
	}
};
struct movable_pawn_info {//important for current move
	using goals_t = std::vector<goal_info>;
	using node_t = typename BoardGraph::node_t;
	using pathfinder_t = typename BoardGraph::pathfinder_t;

	AIPawnProxy* pawn;
	bool has_already_reached_goal;
	goals_t reachable_goals;
	goals_t approachable_goals;//goals which unable to reach, but possible to approach to
	float_t // for all goals can be reached or approached to (ignoring the goal that is already reached if it is):
		max_reachable_distance,
		max_approachable_distance,
		min_reachable_distance,
		min_approachable_distance;

	//TODO: average reachable & approachable distance	
	movable_pawn_info();
	movable_pawn_info(AIPawnProxy& p);

	size_t no_way_alternative_count()const noexcept {
		return pawn->can_move_left() + pawn->can_move_right() + pawn->can_move_up() + pawn->can_move_down();
	}
	size_t possible_alternative_count()const noexcept {
		return std::size(reachable_goals) + std::size(approachable_goals) + no_way_alternative_count();
	}
	void push_alternatives(std::vector<alternative_unique_ptr_t>& alternatives, AlternativesFactory& factory)const;
	// Inefficient alternatives, that must run only when nothing to do else
	void push_no_way_alternatives(std::vector<alternative_unique_ptr_t>& alternatives, AlternativesFactory& factory)const {
		if (pawn->can_move_right())
			alternatives.emplace_back(pawn->get_alternative(factory, pawn->desired_cell.get_right_neighbour()));
		if (pawn->can_move_down())
			alternatives.emplace_back(pawn->get_alternative(factory, pawn->desired_cell.get_bottom_neighbour()));
		if (pawn->can_move_left())
			alternatives.emplace_back(pawn->get_alternative(factory, pawn->desired_cell.get_left_neighbour()));
		if (pawn->can_move_up())
			alternatives.emplace_back(pawn->get_alternative(factory, pawn->desired_cell.get_top_neighbour()));
	}

	void on_goal_reachable(board_cell goal, std::vector<node_t*>& reachable_path, const AIPawnProxy& p) {
		const auto distance = reachable_goals.emplace_back(goal, reachable_path).distnace_to_the_goal(p.desired_cell);
		max_reachable_distance = std::max(max_reachable_distance, distance);
		min_reachable_distance = std::min(min_reachable_distance, distance);
	}
	void on_goal_approachable(board_cell goal, std::vector<node_t*>& unreachable_path, const AIPawnProxy& p) {
		const auto distance = approachable_goals.emplace_back(goal, unreachable_path).distnace_to_the_goal(p.desired_cell);
		max_approachable_distance = std::max(max_approachable_distance, distance);
		min_approachable_distance = std::min(min_approachable_distance, distance);
	}
};
constexpr size_t MaxCollectInfoThreadCount = 0;

std::vector<movable_pawn_info> CollectInfoAboutMovablePawns(std::span<AIPawnProxy> pawns, std::span<board_cell> goals, std::span<board_cell> obstacles);

std::vector<movable_pawn_info> MultithreadedCollectInfoAboutMovablePawns(std::span<AIPawnProxy> pawns, std::span<board_cell> goals, std::span<board_cell> obstacles);
void PrioritizeMovablePawnInfos(std::vector<movable_pawn_info>& infos);
std::vector<alternative_unique_ptr_t> ConvertMovabalePawnInfosToAlternatives(const std::vector<movable_pawn_info>& infos, AlternativesFactory& factory);


struct blocked_pawn_info {//not movable, doesn't matter for current move
	AIPawnProxy* pawn;
	bool has_already_reached_goal;
	std::vector<AIPawnProxy*> owned_blockers;
};