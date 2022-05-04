

// Very typical minimax algorithm implementation.
// The main reason of boilerplate (and a "spagettification") it that version can use multithreading:
//  it runs several of 0-depth level alternatives in a separate threads.  

template<typename Alternative>
utitlity_t Minimax(uint32_t depth, bool maximizer, int64_t max_best_score, int64_t min_best_score, AISituation<Alternative>* current_situation) {
	if (current_situation->end_game())
		return 0;
	if (current_situation->evaluate(maximizer, depth) >= InitialMinimizerValue)//win
		return InitialMinimizerValue;
	if (current_situation->evaluate(maximizer, depth) <= InitialMaximizerValue)//loose
		return InitialMaximizerValue;
	if (depth >= MaxMinimaxDepth)
		return current_situation->evaluate(maximizer, depth);
	using thread_index_t = std::remove_const_t<decltype(MaxMinimaxThreadCount)>;
	if (maximizer) {
		utitlity_t best_score = InitialMaximizerValue;
		auto alternatives = current_situation->get_possible_own_alternatives();
		for (auto& alternative : alternatives) {				
			alternative->Try();
			best_score = std::max(best_score, Minimax<Alternative>(depth + 1, !maximizer, max_best_score, min_best_score, current_situation));
			alternative->Undo();

			max_best_score = std::max(max_best_score, best_score);
			if (min_best_score <= max_best_score)//Alpha Beta Pruning
				break;
		}
		return best_score - (DepthPenalty * depth);
	} else {
		utitlity_t best_score = InitialMinimizerValue;
		auto alternatives = current_situation->get_possible_enemy_alternatives();
		for (auto& alternative : alternatives) {
			alternative->Try();
			best_score = std::min(best_score, Minimax<Alternative>(depth + 1, !maximizer, max_best_score, min_best_score, current_situation));
			alternative->Undo();
			
			min_best_score = std::min(min_best_score, best_score);
			if (min_best_score <= max_best_score)//Alpha Beta Pruning
				break;
		}
		return best_score + (DepthPenalty * depth);
	}
}


template<typename Alternative>
utitlity_t MultithreadedMinimax(bool maximizer, int64_t max_best_score, int64_t min_best_score, AISituation<Alternative>* current_situation) {
	constexpr uint32_t Depth = 0;
	if constexpr (!MaxMinimaxThreadCount)
		return Minimax(Depth, maximizer, max_best_score, min_best_score, current_situation);//Recall to singlethreaded version
	

	if (current_situation->end_game())
		return 0;
	if (current_situation->evaluate(maximizer, Depth) >= InitialMinimizerValue)//win
		return InitialMinimizerValue;
	if (current_situation->evaluate(maximizer, Depth) <= InitialMaximizerValue)//loose
		return InitialMaximizerValue;
	if (Depth >= MaxMinimaxDepth)
		return current_situation->evaluate(maximizer, Depth);


	using thread_index_t = std::remove_const_t<decltype(MaxMinimaxThreadCount)>;
	constexpr size_t TotalThreadCount = (MaxMinimaxThreadCount + 1);
	std::array<std::thread, MaxMinimaxThreadCount> threads;
	std::array<utitlity_t, TotalThreadCount> thread_best_scores;
	std::atomic_uint_fast8_t copies = 0;
	if (maximizer) {
		utitlity_t best_score = InitialMaximizerValue;
		auto alternatives = current_situation->get_possible_own_alternatives();
		const size_t part_size = std::size(alternatives) / TotalThreadCount;
		Expects(part_size * TotalThreadCount <= std::size(alternatives));

		if (part_size) {
			auto situation = current_situation;//to feed lambda
			for (thread_index_t thread_index = 0; thread_index < MaxMinimaxThreadCount; ++thread_index)
				threads[thread_index] = std::thread([&situation, &copies, best_score, part_size, &thread_best_scores, thread_index, maximizer, Depth, max_best_score, min_best_score]()mutable {
					auto current_situation = situation->clone();
					copies.fetch_add(1, std::memory_order_relaxed);
					auto alternatives = current_situation->get_possible_own_alternatives();
					for (size_t a = (thread_index * part_size); a < ((thread_index * part_size) + part_size); ++a) {
						auto& alternative = alternatives[a];
						alternative->Try();
						best_score = std::max(best_score, Minimax<Alternative>(Depth + 1, !maximizer, max_best_score, min_best_score, current_situation.get()));
						alternative->Undo();
						max_best_score = std::max(max_best_score, best_score);
						if (min_best_score <= max_best_score)//Alpha Beta Pruning
							break;
					}
					thread_best_scores[thread_index] = best_score - (DepthPenalty * Depth);
				});
			while (copies.load(std::memory_order_relaxed) < MaxMinimaxThreadCount);//wait untill all workers will have own copies of current_situation_
			for (size_t a = (part_size * MaxMinimaxThreadCount); a < std::size(alternatives); ++a) {
				auto& alternative = alternatives[a];

				alternative->Try();
				best_score = std::max(best_score, Minimax<Alternative>(Depth + 1, !maximizer, max_best_score, min_best_score, current_situation));
				alternative->Undo();

				max_best_score = std::max(max_best_score, best_score);
				if (min_best_score <= max_best_score)//Alpha Beta Pruning
					break;
			}
			const auto this_thread_best_score = best_score - (DepthPenalty * Depth);
			thread_best_scores.back() = this_thread_best_score;
			for (thread_index_t thread_index = 0; thread_index < MaxMinimaxThreadCount; ++thread_index)
				threads[thread_index].join();
			return *std::max_element(std::begin(thread_best_scores), std::end(thread_best_scores));
		} 
		return Minimax<Alternative>(Depth, maximizer, max_best_score, min_best_score, current_situation);// recall to singlethreaded version
	} else {
		utitlity_t best_score = InitialMinimizerValue;
		auto alternatives = current_situation->get_possible_enemy_alternatives();
		const size_t part_size = std::size(alternatives) / TotalThreadCount;
		Expects(part_size * TotalThreadCount <= std::size(alternatives));
		if (part_size) {
			auto situation = current_situation;//to feed lambda
			for (thread_index_t thread_index = 0; thread_index < MaxMinimaxThreadCount; ++thread_index)
				threads[thread_index] = std::thread([situation, &copies, best_score, part_size, &thread_best_scores, thread_index, maximizer, Depth, max_best_score, min_best_score]()mutable {
					auto current_situation = situation->clone();
					copies.fetch_add(1, std::memory_order_relaxed);
					auto alternatives = current_situation->get_possible_enemy_alternatives();
					for (size_t a = (thread_index * part_size); a < ((thread_index * part_size) + part_size); ++a) {
						auto& alternative = alternatives[a];
						alternative->Try();
						best_score = std::min(best_score, Minimax<Alternative>(Depth + 1, !maximizer, max_best_score, min_best_score, current_situation.get()));
						alternative->Undo();
						min_best_score = std::min(min_best_score, best_score);
						if (min_best_score <= max_best_score)//Alpha Beta Pruning
							break;
					}
					thread_best_scores[thread_index] = best_score + (DepthPenalty * Depth);
				});
			while (copies.load(std::memory_order_relaxed) < MaxMinimaxThreadCount);//wait untill all workers will have own copies of current_situation_
			for (size_t a = (part_size * MaxMinimaxThreadCount); a < std::size(alternatives); ++a) {
				auto& alternative = alternatives[a];
				alternative->Try();
				best_score = std::min(best_score, Minimax<Alternative>(Depth + 1, !maximizer, max_best_score, min_best_score, current_situation));
				alternative->Undo();
				min_best_score = std::min(min_best_score, best_score);
				if (min_best_score <= max_best_score)//Alpha Beta Pruning
					break;
			}
			const auto this_thread_best_score = best_score + (DepthPenalty * Depth);
			thread_best_scores.back() = this_thread_best_score;
			for (thread_index_t thread_index = 0; thread_index < MaxMinimaxThreadCount; ++thread_index)
				threads[thread_index].join();
			return *std::min_element(std::begin(thread_best_scores), std::end(thread_best_scores));
		} 
	}
	return Minimax<Alternative>(Depth, maximizer, max_best_score, min_best_score, current_situation);// recall to singlethreaded version
}