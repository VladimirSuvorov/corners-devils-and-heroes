#pragma once
#include <random>
#include "../../Models/Pawn.hpp"
#include  "../Player.hpp"
#include "AI/DescisionMaking/Minimax.hpp"
#include "AI/AIBoardSituation.hpp"




class CrazyMadInsaneAIPlayer ://As crazy as possible for AI...
	public Player
{
public:
	CrazyMadInsaneAIPlayer() = default;
	void OnStartGame(Board& board, std::span<Pawn> own, std::span<Pawn> enemy) {
		std::for_each(std::begin(own), std::end(own), [this](Pawn& pawn) {
			pawn.reset_owner(this);
		});
		current_situation_.start_game(board, own, enemy);
	}
	success_t make_move()noexcept final {
		current_situation_.on_enemy_have_made_move();
		if (auto solution = select_best_action()) {
			solution->Redo();
			return true;
		}
		LOG_INFO("AI didn't select an action to do\n");
		Expects(false);
		return false;
	}

	~CrazyMadInsaneAIPlayer()noexcept = default;
private:
	alternative_unique_ptr<AIPawnActionAlternative> select_best_action() {
		alternative_unique_ptr<AIPawnActionAlternative> best_alternative = nullptr;
		auto alternatives = current_situation_.get_possible_own_alternatives();
		Expects(!std::empty(alternatives));
		if (use_random_action()) {
			const size_t action_index = select_random_action(std::size(alternatives) - 1);
			Expects(std::size(alternatives) > action_index);
			best_alternative = std::move(alternatives[action_index]);
		} else {
			int best_score = InitialMaximizerValue; 
			for (auto& alternative : alternatives) {
				alternative->Try();
				int alternative_score = MultithreadedMinimax(false, InitialMaximizerValue, InitialMinimizerValue, &current_situation_);//0, false, Min, Max
				alternative->Undo();
				if (alternative_score > best_score) {
					best_score = alternative_score;
					best_alternative = std::move(alternative);
				}
			}
		}
		return best_alternative;
	}
	//to prevent AI from being too rational (and so prdictible), sometimes, AI have to make a random (and - even, probably - stupid) moves.
	static bool use_random_action() {
		static std::random_device device;
		static std::mt19937_64 engine(std::seed_seq{ device(), device(), device(), device(), device() });
		static std::bernoulli_distribution distribution(0.15);
		return distribution(engine);
	}

	static size_t select_random_action(size_t MaxActionIndex) {
		static std::random_device device;
		static std::mt19937_64 engine(std::seed_seq{ device(), device(), device(), device(), device(), device() });

		static std::normal_distribution<float_t> distribution(0.f, 10000.f);

		// Todo: sometimes prefer first alternative
		// TODO: prefer to achieve a goal or goto goal
		// Random (chance, range): 1/20 1; 1/4 1-5; 1/5 1-20; 1/10 1-100
		// 324 = 9 pawns * 4 moves * 9 goals

		const auto probability = distribution(engine) / 100.f;
		if (probability > 90.f)
			return 0;
		if (probability > 70.f)
			return std::clamp<size_t>(std::uniform_int_distribution(0, 5)(engine), 0, MaxActionIndex);
		if (probability > 50.f)
			return std::clamp<size_t>(std::uniform_int_distribution(0, 20)(engine), 0, MaxActionIndex);
		if (probability > 30.f)
			return std::clamp<size_t>(std::uniform_int_distribution(0, 45)(engine), 0, MaxActionIndex);
		if (probability > 10.f)
			return std::clamp<size_t>(std::uniform_int_distribution(0, 100)(engine), 0, MaxActionIndex);
		return std::clamp<size_t>(std::uniform_int_distribution(0, int(MaxActionIndex))(engine), 0, MaxActionIndex);
	}
private:
	AIBoardSituation current_situation_;
};
