#pragma once
// This file contains very basic game rules, types and constants.

#include <array>
#include <type_traits>
#include <algorithm>
#include <gsl/gsl>
#include <inttypes.h>
#include "GameRules.hpp"
#include "../utilities.h"

constexpr uint8_t BoardWidth = 8;
constexpr uint8_t PawnCountPerPlayer = 9;
constexpr uint8_t GoalCountPerPlayer = PawnCountPerPlayer;
constexpr uint8_t PlayerCount = 2;
using player_index_t = uint8_t;
using player_count_t = player_index_t;

//Why don't use std::bitset? 
//1) For board_cell bitset is overkill: to store x & y in range [0; 7] (8 values in total for each component) 8bits is actually enough: 
//	no need to use 32 or - even worse - 64 bit integers. 
//2) For board_state  
using mask_t = uint64_t;
using board_row_t = uint8_t;
using player_pawn_masks_t = std::array<mask_t, PlayerCount>;
using input_player_pawn_masks_t = std::span<mask_t>;

using success_t = bool;

class board_cell {
public:
#if 0
	uint8_t x : 3; // Ѕитовые пол€ напросились из того факта, что дл€ хранени€ координат 8x8 достаточно 3х бит на координату.
	uint8_t y : 3; //  ак и у любого другого решени€, у данного были и плюсы и минусы. ќтчасти, можно считать данное решение "экономией на спичках"/прихотью.
	uint8_t unused : 2;
#endif
	// bit order: BYYYAXXX where A, B means nothing (but could be a sign), and YYY and XXX - bits are cell position components (x, y - respectively) at 2d board
	static constexpr uint8_t MeaningfulBits = 0b00000111;
	static constexpr uint8_t XLeftBitShift = 0;
	static constexpr uint8_t YLeftBitShift = 4;
	static constexpr uint8_t XMask = 0b00000111;// = MeaningfulBits << XLeftBitShift
	static constexpr uint8_t YMask = 0b01110000;// = MeaningfulBits << YLeftBitShift



	//получение соседней клетки может вернуть текущую, если соседн€€ клетка не существует (шахматна€ доска = 8x8 клеток)
	constexpr board_cell get_top_neighbour()const {
		return has_top_neighbour() ? board_cell(x(), y() - 1) : *this;
	}
	constexpr board_cell get_bottom_neighbour()const {
		return has_bottom_neighbour() ? board_cell(x(), y() + 1) : *this;
	}
	constexpr board_cell get_left_neighbour()const {
		return has_left_neighbour() ? board_cell(x() - 1, y()) : *this;
	}
	constexpr board_cell get_right_neighbour()const {
		return has_right_neighbour() ? board_cell(x() + 1, y()) : *this;
	}
	constexpr bool is_neighbour(board_cell cell)const noexcept;

	constexpr bool has_top_neighbour()const noexcept { return y() > 0; }
	constexpr bool has_bottom_neighbour()const noexcept { return y() < 7; }
	constexpr bool has_left_neighbour()const noexcept { return x() > 0; }
	constexpr bool has_right_neighbour()const noexcept { return x() < 7; }

	constexpr board_cell& x(uint8_t xvalue)noexcept {
		Expects(this);
		masked_value_ = (masked_value_ & YMask)|((xvalue & MeaningfulBits) << XLeftBitShift);// new_value = { param_x, prev_value.y }
		return *this;
	}
	constexpr board_cell& y(uint8_t yvalue)noexcept {
		Expects(this);
		masked_value_ = (masked_value_ & XMask)|((yvalue & MeaningfulBits) << YLeftBitShift);// new_value = { prev_value.x, param_y }
		return *this;
	}
	constexpr uint8_t x()const noexcept {
		Expects(this);
		return (masked_value_ & XMask) >> XLeftBitShift;
	}
	constexpr uint8_t y()const noexcept {
		Expects(this);
		return (masked_value_ & YMask) >> YLeftBitShift;
	}
	constexpr uint8_t masked_value()const noexcept {
		Expects(this);
		return masked_value_;
	}

	board_cell& xy(uint8_t xvalue, uint8_t yvalue)noexcept {
		Expects(this);
		masked_value_ = ((xvalue& MeaningfulBits) << XLeftBitShift) | ((yvalue& MeaningfulBits) << YLeftBitShift);
		return *this;
	}

	constexpr board_cell(uint8_t masked_value = 0)noexcept :
		masked_value_(masked_value)
	{}


	constexpr board_cell(uint8_t xvalue, uint8_t yvalue)noexcept :
		masked_value_(((xvalue& MeaningfulBits) << XLeftBitShift) | ((yvalue & MeaningfulBits) << YLeftBitShift))
	{}

	constexpr board_cell(const board_cell& oth) :
		masked_value_(oth.masked_value_)
	{
		Expects(this && &oth);
	}
	
	constexpr board_cell(board_cell&& src)noexcept :
		masked_value_(src.masked_value_)
	{
		Expects(this && &src);
	}
	board_cell& operator=(const board_cell& oth) {
		Expects(this && &oth);
		masked_value_ = oth.masked_value();
		return *this;
	}
	board_cell& operator=(board_cell&& src) {
		Expects(this && &src);
		masked_value_ = src.masked_value();
		return *this;
	}
	~board_cell() = default;
private:
	uint8_t masked_value_;
};

//static_assert(sizeof(board_cell) == 1);

constexpr bool operator < (board_cell lhs, board_cell rhs)noexcept {
	return lhs.y() != rhs.y() ? lhs.y() < rhs.y() : lhs.x() < rhs.x();
}
constexpr bool operator == (board_cell lhs, board_cell rhs)noexcept {
	return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}
constexpr bool operator != (board_cell lhs, board_cell rhs)noexcept {
	return !(lhs == rhs);
}

constexpr bool board_cell::is_neighbour(board_cell cell)const noexcept {
	if (has_top_neighbour() && (get_top_neighbour() == cell))
		return true;
	if (has_bottom_neighbour() && (get_bottom_neighbour() == cell))
		return true;
	if (has_left_neighbour() && (get_left_neighbour() == cell))
		return true;
	if (has_right_neighbour() && (get_right_neighbour() == cell))
		return true;
	return false;
}


class board_state {
public:
	// 64 bits (8 bytes) gives an ability to encode all the board with mask so,
	// if a bit is set (=1) it means that at position - { bit position in a byte, the byte position in 8-byte (64 bit) integer } - a pawn stands.
	// To distinct if a given pawn (a bit set to = 1 somwhere at 64bit int) belongs to specific player, the 2 masks are used:
	// due to the nature of bitmasks it is easily to combine them via disjunction, so test if a pawn (ignoring player whose that pawn) stands in a given cell.
	// On the other hand, as far as the game rules says that there only 2 players maximum and all the board 8 * 8 = 64 cells in size,
	// using a 64bit integer looks like a good idea.
	board_state() {
		clear();
	}

	board_state(input_player_pawn_masks_t player_pawn_masks) {
		player_pawn_masks = input_player_pawn_masks_t{ std::data(player_pawn_masks), std::min(std::size(player_pawn_mask_), std::size(player_pawn_masks)) };
		std::copy(std::begin(player_pawn_masks), std::end(player_pawn_masks), std::begin(player_pawn_mask_));
		debug_check();
	}
	constexpr board_state(player_pawn_masks_t player_pawn_masks) :
		player_pawn_mask_(player_pawn_masks)
	{
		debug_check();
	}
	constexpr board_state(board_state&& src) :
		player_pawn_mask_(src.player_pawn_mask_)
	{
		debug_check();
	}
	constexpr board_state(const board_state& oth) :
		player_pawn_mask_(oth.player_pawn_mask_)
	{
		debug_check();
	}
	constexpr board_state& operator=(board_state&& src) {
		player_pawn_mask_ = src.player_pawn_mask_;
		debug_check();
		return *this;
	}
	constexpr board_state& operator=(const board_state& oth) {
		player_pawn_mask_ = oth.player_pawn_mask_;
		debug_check();
		return *this;
	}
	~board_state() = default;
public://creation
	void clear() {
		memset(player_pawn_mask_.data(), 0, sizeof(mask_t) * std::size(player_pawn_mask_));
	}
	bool try_emplace_pawn(player_index_t player_index, board_cell pawn) {
		if (empty(pawn)) {
			player_pawn_mask_[player_index] |= cell_to_mask(pawn);
			return true;
		}
		return false;
	}
	void replace_pawn(player_index_t player_index, board_cell pawn) {
		if (has_player_pawn(player_index, pawn))
			player_pawn_mask_[player_index] &= ~cell_to_mask(pawn);
	}
public://utilities
	constexpr static mask_t cell_to_mask(board_cell cell) noexcept {
		return (mask_t(1) << ((BoardWidth * cell.y()) + cell.x()));
	}
	constexpr mask_t possible_moves_mask(board_cell from)const noexcept {
		return (cell_to_mask(from.get_left_neighbour()) | cell_to_mask(from.get_top_neighbour()) |
			cell_to_mask(from.get_right_neighbour()) | cell_to_mask(from.get_bottom_neighbour()))
			& (~cell_to_mask(from));// get_*_neighbour returns neighbour cell only if such cell exist; otherwise - returns *this;
	}
public://emptiness
	constexpr bool empty(board_cell cell) const noexcept {
		return !has_pawn(cell);
	}
	constexpr mask_t all_empty_mask()const noexcept {
		return ~all_pawn_mask();
	}
	constexpr mask_t all_except_player_empty_mask(player_index_t player_index) const noexcept {
		return ~all_except_player_pawn_mask(player_index);
	}
	constexpr mask_t all_empty_except_mask(board_cell exceptional_cell) const noexcept {
		return ~all_pawns_except_mask(exceptional_cell);
	}

	constexpr bool has_pawn(board_cell cell) const noexcept {
		return all_pawn_mask() & cell_to_mask(cell);
	}
	constexpr mask_t all_pawn_mask() const noexcept {
		mask_t result_mask = 0;
		for (auto& player_mask : player_pawn_mask_)
			result_mask |= player_mask;
		return result_mask;
	}
	constexpr mask_t all_except_player_pawn_mask(player_index_t player_index) const noexcept {
		mask_t result_mask = 0;
		for (player_count_t p = 0; p < std::size(player_pawn_mask_); ++p)
			if (p != player_index)
				result_mask |= player_pawn_mask_[p];
		return result_mask;
	}
	constexpr mask_t all_pawns_except_mask(board_cell exceptional_cell) const noexcept {
		return all_pawn_mask() & (~cell_to_mask(exceptional_cell));
	}

public://player
	constexpr player_count_t player_of_pawn_at(board_cell cell) const noexcept {
		auto cell_mask = cell_to_mask(cell);
		for (player_count_t p = 0; p < std::size(player_pawn_mask_); ++p)
			if (player_pawn_mask_[p] & cell_mask)
				return p;
		return (~0);
	}

	constexpr bool has_player_pawn(player_index_t player_index, board_cell cell)const noexcept {
		return player_pawn_mask_[player_index] & cell_to_mask(cell);
	}
	constexpr mask_t player_pawn_mask(player_index_t player_index) const noexcept {
		return player_pawn_mask_[player_index];
	}
	constexpr mask_t player_pawn_except_mask(player_index_t player_index, board_cell exceptional_cell) const noexcept {
		return player_pawn_mask_[player_index] & (~cell_to_mask(exceptional_cell));
	}
public://pawn movement
	bool try_move_pawn(board_cell from, board_cell to) noexcept {
		if (can_move_pawn(from, to)) {
			const auto player = player_of_pawn_at(from);
			player_pawn_mask_[player] = (player_pawn_mask_[player] & ~cell_to_mask(from)) | cell_to_mask(to);
			debug_check();

#if 0
			printf("###############\n");
			debug_display();
			printf("from { x = %d, y = %d } to { x = %d, y = %d }\n",
				int(from.x()),
				int(from.y()),
				int(to.x()),
				int(to.y()));
			printf("###############\n");
#endif
			return true;
		}
		return false;
	}

	constexpr bool is_pawn_movable(board_cell at)const noexcept {
		return (has_pawn(at)) ? possible_moves_mask(at) & all_empty_mask() : false;
	}
	constexpr bool is_pawn_blocked(board_cell at)const noexcept {
		return (has_pawn(at)) ? possible_moves_mask(at) & all_pawn_mask() : false;
	}
	constexpr bool can_move_pawn(board_cell from, board_cell to)const noexcept {
		auto to_position_mask = cell_to_mask(to);
		if (has_pawn(from) && (!(all_pawn_mask() & to_position_mask)))
			return possible_moves_mask(from) & to_position_mask;

		return false;
	}
	constexpr bool can_move_left(board_cell from)const noexcept {
		return from.has_left_neighbour() ? can_move_pawn(from, from.get_left_neighbour()) : false;
	}
	constexpr bool can_move_up(board_cell from)const noexcept {
		return from.has_top_neighbour() ? can_move_pawn(from, from.get_top_neighbour()) : false;
	}
	constexpr bool can_move_right(board_cell from)const noexcept {
		return from.has_right_neighbour() ? can_move_pawn(from, from.get_right_neighbour()) : false;
	}
	constexpr bool can_move_down(board_cell from)const noexcept {
		return from.has_bottom_neighbour() ? can_move_pawn(from, from.get_bottom_neighbour()) : false;
	}


public:
	void debug_display()const {
		std::array<char, 9 * 8> grid;
		memset(std::data(grid), '-', std::size(grid) * sizeof(decltype(grid)::value_type));
		for (uint8_t row = 0; row < 8; ++row)
			grid[(9 * row) + 8] = '\0';


		constexpr uint8_t BitsPerByte = BoardWidth;
		Expects(!(BoardWidth & 1));//BoardWidth must be a power of 2 for correct computation of modulo via &-bit operator;
		// but here we check if it is even in hope it never will be - for example - 6 :)]

#if 0
		for (uint8_t y = 0; y < BoardWidth; ++ y)
			for (uint8_t x = 0; x < BoardWidth; ++x)
				for (player_count_t p = 0; p < std::size(player_pawn_mask_); ++p)
					if (has_player_pawn(p,{x,y}))
						grid[y * (BoardWidth + 1) + x] = 'A' + p;
#endif


		for (uint8_t b = 0; b < BitsPerByte * sizeof(mask_t); ++b) {
			//bit magic is:
			// 1) get y value (b >> 3), 
			// 2) multiply it by 9 (because char per cell is used to display a pawn pos + 1 byte over as '\0')
			// 3) then add remains: %BoardWidth = %8 = &8 = &BoardWidth
			//auto& cell = grid[((b >> 3) * 9) | (b & (BoardWidth - 1))];
			auto& cell = grid[b + (b /BitsPerByte)];
			mask_t pawn_mask = (mask_t(1) << b);
			for (player_count_t p = 0; p < std::size(player_pawn_mask_); ++p)
				if (player_pawn_mask_[p] & pawn_mask)
					cell = 'A' + p;
		}
		printf("========\n");
		for (uint8_t row = 0; row < 8; ++row)
			printf("%s\n", std::data(grid) + (9 * row));
		printf("========\n");
	}
private:
	constexpr void debug_check() const {
		for (player_count_t p = 0; p < std::size(player_pawn_mask_); ++p) {
			auto other_pawn_mask = all_except_player_pawn_mask(p);
			Expects(!(other_pawn_mask & player_pawn_mask_[p]));// not overlaps
		}
	}

private:
	player_pawn_masks_t player_pawn_mask_;
};

class GameBoard {
public:

	static const std::array<board_cell, PawnCountPerPlayer>& GetInitialPlayerPawnPositions(player_index_t player_index) {
		if (initialized_)
			return initial_player_pawn_positions_[player_index];
		else {
			Initialize();
			return GetInitialPlayerPawnPositions(player_index);
		}
	}

	static const std::array<board_cell, PawnCountPerPlayer>& GetPlayerPawnGoalPositions(player_index_t player_index) {
		if (initialized_)
			return player_pawn_goal_positions_[player_index];
		else {
			Initialize();
			return GetPlayerPawnGoalPositions(player_index);
		}
	}

private:
	static void Initialize() {
		////////////////////////////////////////////
		//
		//	[S][S][S][ ][ ][ ][ ][ ]	(7) -> (0)
		//	[S][S][S][ ][ ][ ][ ][ ]	(6) -> (1)
		//	[S][S][S][ ][ ][ ][ ][ ]	(5) -> (2)
		//	[ ][ ][ ][ ][ ][ ][ ][ ]	(4) -> (3)
		//	[ ][ ][ ][ ][ ][ ][ ][ ]	(3) -> (4)
		//	[ ][ ][ ][ ][ ][F][F][F]	(2) -> (5)
		//	[ ][ ][ ][ ][ ][F][F][F]	(1) -> (6)
		//	[ ][ ][ ][ ][ ][F][F][F]	(0) -> (7)
		//								F - first_player
		//	(0)(1)(2)(3)(4)(5)(6)(7)	S - second_player
		////////////////////////////////////////////
		initial_player_pawn_positions_[0] =
		{
			board_cell{ 5, 5 }, board_cell{ 6, 5 }, board_cell{ 7, 5 },
			board_cell{ 5, 6 }, board_cell{ 6, 6 }, board_cell{ 7, 6 },
			board_cell{ 5, 7 }, board_cell{ 6, 7 }, board_cell{ 7, 7 }
		};
		initial_player_pawn_positions_[1] =
		{
			board_cell{ 0, 0 }, board_cell{ 1, 0 }, board_cell{ 2, 0 },
			board_cell{ 0, 1 }, board_cell{ 1, 1 }, board_cell{ 2, 1 },
			board_cell{ 0, 2 }, board_cell{ 1, 2 }, board_cell{ 2, 2 }
		};
		for (player_index_t p = 0; p < PlayerCount; ++p)
			player_pawn_goal_positions_[p] = initial_player_pawn_positions_[(PlayerCount - 1) - p];
		initialized_ = true;
	}
private:
	inline static std::array<std::array<board_cell, PawnCountPerPlayer>, PlayerCount> initial_player_pawn_positions_;
	inline static std::array<std::array<board_cell, PawnCountPerPlayer>, PlayerCount> player_pawn_goal_positions_;
	inline static bool initialized_ = false;
};
