
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include "GraphicApplication.hpp"

GraphicApplication::GraphicApplication(uint32_t window_width, uint32_t window_height, const char* window_title, GameView& view)noexcept :
	frame_(view),
	main_window_(window_width, window_height, window_title, frame_)
{

}

GraphicApplication::~GraphicApplication() {
	update_thread_.join();
}

void GraphicApplication::close()noexcept {
	LOG_INFO("closed\n");
	is_running_.store(false, std::memory_order_release);

	while (!update_thread_finished_.load(std::memory_order_acquire));
}

void GraphicApplication::run()noexcept {
	LOG_INFO("run\n");
	is_running_.store(true, std::memory_order_seq_cst);

	update_thread_ = std::thread(&GraphicApplication::threaded_update, this);
	Expects(update_thread_.joinable());

	LOG_INFO("running\n");
	while (is_running_.load(std::memory_order_acquire)) {
		main_window_.poll_events();
		OnWindowUpdate();
		if (main_window_.should_close())
			OnWindowTriesToClose();

	}
}

void GraphicApplication::threaded_update()noexcept {
	update_thread_finished_.store(false, std::memory_order_release);
	while (is_running_.load(std::memory_order_acquire))
		OnTick();
	update_thread_finished_.store(true, std::memory_order_release);
}

