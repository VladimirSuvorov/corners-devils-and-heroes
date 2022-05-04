#include "AIBoardSituation.hpp"

void AIBoardSituation::start_game(Board& board, std::span<Pawn> my, std::span<Pawn> enemy) {
	actual_board_ = &board;
	expected_board_state_ = actual_board_->get_state();
	start_player(My, my, enemy);
	start_player(Enemy, enemy, my);
	printf("GameStarted\n");
	expected_board_state_.debug_display();
}

int64_t AIBoardSituation::evaluate(bool maximizer, int depth)noexcept {
	const uint8_t my_reached_goal_count = reached_goal_pawn_count(My);

	if (my_reached_goal_count >= GoalCountPerPlayer)
		return InitialMinimizerValue;

	const uint8_t enemy_reached_goal_count = reached_goal_pawn_count(Enemy);

	if (enemy_reached_goal_count >= GoalCountPerPlayer)
		return InitialMaximizerValue;

	auto my_freedom_value = current_freedom_value(My);
	auto my_max_freedom_value = max_freedom_value(My);
	auto enemy_freedom_value = current_freedom_value(Enemy);
	auto enemy_max_freedom_value = max_freedom_value(Enemy);




	auto my_average_distance = average_distance_to_goals(My);

	auto enemy_average_distance = average_distance_to_goals(Enemy);

	constexpr uint16_t OwnReachedGoalCost = 25000;
	constexpr uint16_t EnemyReachedGoalCost = 20000;

	constexpr uint16_t OwnPawnFreedomCost = 800;
	constexpr uint16_t EnemyPawnFreedomCost = 900;

	constexpr uint16_t OwnDistanceShortageCost = 5000;
	constexpr uint16_t EnemyDistanceShortageCost = 400;
	constexpr uint16_t OwnDistanceRemainCost = 3500;
	constexpr uint16_t EnemyDistanceRemainCost = 400;



	return int32_t(OwnReachedGoalCost * my_reached_goal_count) -
		int32_t(EnemyReachedGoalCost * enemy_reached_goal_count) +
		int32_t(EnemyReachedGoalCost * ((std::size(goals_[My]) - my_reached_goal_count))) -
		int32_t(OwnReachedGoalCost * ((std::size(goals_[Enemy]) - enemy_reached_goal_count))) +
		int32_t(my_max_freedom_value ? (OwnPawnFreedomCost * my_freedom_value / my_max_freedom_value) : 0)-
		int32_t(enemy_max_freedom_value ? (EnemyPawnFreedomCost * enemy_freedom_value / enemy_max_freedom_value) : 0) +
		int32_t(enemy_freedom_value ? (OwnPawnFreedomCost * enemy_max_freedom_value / enemy_freedom_value) : 0) -
		int32_t(my_freedom_value ? (EnemyPawnFreedomCost * my_max_freedom_value / my_freedom_value) : 0) +
		int32_t(OwnDistanceShortageCost * (16 - my_average_distance)) -
		int32_t(EnemyDistanceShortageCost * (16 - enemy_average_distance)) +
		int32_t(OwnDistanceRemainCost * enemy_average_distance) -
		int32_t(EnemyDistanceRemainCost * my_average_distance) -
		int32_t(DepthPenalty * (maximizer ? depth : -depth));
}