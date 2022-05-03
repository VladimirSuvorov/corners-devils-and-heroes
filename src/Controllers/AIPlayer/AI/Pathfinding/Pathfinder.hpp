#pragma once
#include <vector>
#include <iterator>
#include <algorithm>
#include "../../../../utilities.h"



template<typename Position>
class AStarNode {// AStarNode - a node class for A* pathfinding algorithm.
public:
	using position_type = Position;

	AStarNode() :
		f_(0.0), g_(0.0), h_(0.0),
		closed_(false), open_(false)
	{}

	virtual ~AStarNode() = default;

	void set_f(float f) {
		f_ = f;
	}

	void set_g(float g) {
		g_ = g;
	}

	void set_h(float h) {
		h_ = h;
	}

	void set_open(bool value) {
		open_ = value;
	}

	void set_closed(bool value) {
		closed_ = value;
	}
	float get_f() const {
		return f_;
	}

	float get_g() const {
		return g_;
	}

	float get_h() const {
		return h_;
	}

	bool is_open() const {
		return open_;
	}

	bool is_closed() const {
		return closed_;
	}
	float_t distance_to(AStarNode<Position>* node) const {
		return position_.distance_to(node->position_);
	}

	void reset() {
		open_ = closed_ = false;
		f_ = g_ = h_ = 0.0f;
		parent_ = nullptr;
	}

public:

	void set_parent(AStarNode* parent) {
		parent_ = parent;
	}
	AStarNode* get_parent() const {
		return parent_;
	}

	void add_child(AStarNode& child, float distance) {
		bound_nodes_.emplace_back(std::make_pair(&child, distance));
	}

	std::vector<std::pair<AStarNode*, float>>& get_children() {
		return bound_nodes_;
	}
	void set_position(position_type position) {
		position_ = position;
	}
	position_type position()const {
		return position_;
	}
protected:
	float f_ = 0.f, g_ = 0.f, h_ = 0.f;
	bool open_ = false, closed_ = false;


protected:

	void clear_children() {
		bound_nodes_.clear();
	}
private:
	AStarNode* parent_ = nullptr;
	std::vector<std::pair<AStarNode*, float>> bound_nodes_;
	Position position_;
};

template<typename Position>
struct node_comparator {// node_comparator - comparator for A* algorithm.
	bool operator() (const AStarNode<Position>* s1, const AStarNode<Position>* s2) const {
		return s1->get_f() < s2->get_f();
	}
};

template<typename Position>
class AStar {// AStar - is an implementation of A* pathfinding algorithm
public:

	using node_type = AStarNode<Position>;
	float distance_between(node_type* from, node_type* to) const {
		return from->distance_to(to);
	}

	void build_path(node_type* node, std::vector<node_type*>& path) {
		node_type* parent = node->get_parent();
		path.emplace_back(node);
		while (parent) {//TODO: rewrite with for
			path.emplace_back(parent);
			parent = parent->get_parent();
		}
	}

	bool get_path(node_type* from, node_type* to, std::vector<node_type*>& path, std::vector<node_type*>* incomplete_path = nullptr);
	void clear() {
		release_nodes();
		open_.clear();
		closed_.clear();
	}



	AStar() = default;
	~AStar() = default;


	static AStar& get_instance() {
		static AStar instance;
		return instance;
	}

private:


	void release_nodes() {
		for (const auto& node : open_)
			node->reset();
		for (const auto& node : closed_)
			node->reset();
	}
	void push_open(node_type* node) {
		Expects(node);
		open_.emplace_back(node);
		std::push_heap(std::begin(open_), std::end(open_), node_comparator<Position>());
		node->set_open(true);
	}
	void pop_open(node_type* node) {
		Expects(node);
		std::pop_heap(std::begin(open_), std::end(open_), node_comparator<Position>());
		open_.pop_back();
		node->set_open(false);
	}

	std::vector<node_type*> open_, closed_;
};



template <class NodeT>
class PathFinder {//PathFinder finds path from origin to destination using given algorithm. It's find_path method takes an A* or any other algorithm.
public:

	explicit PathFinder() :
		origin_(nullptr), destination_(nullptr)
	{}

	void set_origin(NodeT& start) {
		origin_ = &start;
	}

	void set_destination(NodeT& destination) {
		destination_ = &destination;
	}

	NodeT* get_origin() const {
		return origin_;
	}

	NodeT* get_destination() const {
		return destination_;
	}

	template <class TAlgorithm>
	bool find_path(TAlgorithm& algorithm, std::vector<NodeT*>& solution, std::vector<NodeT*>* incomplete_solution = nullptr, size_t hint = 0) {
		std::vector<typename TAlgorithm::node_type*> path, incomplete_path;

		if (hint > 0) {
			path.reserve(hint);
			if (incomplete_solution)
				incomplete_path.reserve(hint);
		}

		Expects(origin_ && destination_);
		if (algorithm.get_path(origin_, destination_, path, &incomplete_path)) {
			solution.clear();
			solution.reserve(std::size(path));

			std::transform(std::rbegin(path), std::rend(path), std::back_inserter(solution), [](typename TAlgorithm::node_type* node)->NodeT* {
				return static_cast<NodeT*>(node);
			});
			return true;
		} else
			if (incomplete_solution) {
				incomplete_solution->clear();
				incomplete_solution->reserve(std::size(incomplete_path));

				std::transform(std::rbegin(incomplete_path), std::rend(incomplete_path), std::back_inserter(*incomplete_solution),
					[](typename TAlgorithm::node_type* node)->NodeT* {
						return static_cast<NodeT*>(node);
					});
			}
		return false;
	}

private:
	NodeT *origin_, *destination_;
};

#include "Pathfinder.inl"