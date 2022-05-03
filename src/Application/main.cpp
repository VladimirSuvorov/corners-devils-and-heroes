
#include "../Controllers/Game.hpp"

#include "../Controllers/HumanPlayer.hpp"
#include "../Controllers/AIPlayer/StyledAIPlayer.hpp"

#include "../Views/TiledBoardView.hpp"
#include "../Views/GameScreen.hpp"
#include "../Views/DefaultGameView.hpp"

#include "../Application/GraphicApplication.hpp"

#define AI_VS_HUMAN // Alternative case - AI_VS_AI (there is no such macro: just undef this one)

class GameApplication :
	public GraphicApplication
{
public:
	GameApplication()noexcept :
		GraphicApplication(640, 480, "Corners", view_),
		view_(background_),
		devil_style_(view_, { -1, 1, 0.25, 0.25 }, { 0, 1, 0.25f, 0.1f }),
		hero_style_(view_, { -1, 1, 0.25, 0.25 }, { 0, 1.f - 0.1f, 0.25f, 0.1f }),
#ifndef AI_VS_HUMAN
		hero_player_(hero_style_),
#else
		hero_player_(get_main_window(), hero_style_),
#endif
		devil_player_(devil_style_),
		players_{&hero_player_, &devil_player_ },
		game_(players_)

	{
		if (show_how_to_play_)
			view_.attach_drawable(tutorial_); 
	}
protected:
	void OnTick()noexcept final {
		update_game();
	}
	void OnWindowUpdate()noexcept final {
		if (get_main_window().is_key_pressed('ESC'))
			close();		
	}
	void OnWindowTriesToClose()noexcept final {
		close();
	}
private:
	void update_game() {
		if (!game_.game_finished()) {
			if (game_.turn_forward())
				show_how_to_play_ = false;
			if (!show_how_to_play_)
				view_.detach_drawable(tutorial_);
		//	std::this_thread::sleep_for(125ms);
		} else
			if (game_.current_player_blocked())
				view_.attach_drawable(no_more_moves_);
			else
				if (auto winner = game_.get_winner())
					if (winner == &hero_player_)
						view_.attach_drawable(win_);
					else
						if (winner == &devil_player_)
							view_.attach_drawable(loose_);
						else abort();
	}
private:
	TiledBoardView background_;
	WinScreen win_;
	LooseScreen loose_;
	NoMoreMovesScreen no_more_moves_;
	TutorialScreen tutorial_;
	bool show_how_to_play_ = false;
private:
	DefaultGameView view_;
private:
	StyledAIPlayer devil_player_; 
#ifndef AI_VS_HUMAN
	StyledAIPlayer hero_player_;
#else
	DefaultHumanPlayer hero_player_;
#endif
	BasicPlayerPawnStyle<AnimatedPawnView> devil_style_;
	BasicPlayerPawnStyle<AnimatedPawnView> hero_style_;
	std::array<Player*, PlayerCount> players_;
	Game game_;
};


static deferred<GameApplication> game;
int main(int argc, char* argv[]) {
	game.create().run();
	game.destroy();
	return 0;
}

