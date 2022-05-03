#pragma once
#include <thread>
#include <atomic>
#include <float.h>

// This file contains minimal IO for the game

/*
* Frame interface.
* Each time RenderWindow's render thread is ready to do some graphic API calls,
* the thread calls Frame::render_callback method. How does Frame implemented - doesn't matter (it, even, could be noop - as default implementation),
* but all graphics API calls must be inside it's render method, and all manipulation with graphics must be done manipulating some specific implementation of the Frame interface.
* CAUTION: Frame could be called from a separate thread.
*/
class Frame {
public:
	// Lifetime of a frame: initialize once -> render for X times -> dispose once && forever
	// All those functions are called from a render thread

	void initialize_callback() { OnInitialize(); }
	void render_callback() { OnRender(); }
	void dispose_callback() { OnDispose(); }
private:
	virtual void OnRender() {}
	virtual void OnInitialize() {}
	virtual void OnDispose() {}
};
class MouseEventHandler {
public:
	virtual void OnMouseMove(double_t x, double_t y) = 0;
	virtual void OnButtonChangesState(int mouse_button_index, bool pressed) = 0;
};
class RenderWindow {
public:

	void close();
	void poll_events();
	bool should_close()const;
	bool is_key_pressed(char32_t Key)const;
	std::pair<uint32_t, uint32_t> size()const;

public:// Frame object must exist during RenderWindow lifetime
	RenderWindow(uint32_t width, uint32_t height, const char* title, Frame& frame);
	void set_mouse_event_handler(MouseEventHandler* event_handler = nullptr) {
		event_handler_ = event_handler;
	}
	MouseEventHandler* get_mouse_event_handler() {
		return event_handler_;
	}
	~RenderWindow();


private:
	void threaded_render();
	void lock()const noexcept {
		while (lock_.test_and_set(std::memory_order_acquire));
	}
	bool try_lock()const noexcept {
		return (!lock_.test_and_set(std::memory_order_acquire));
	}
	void unlock()const noexcept {
		lock_.clear(std::memory_order_release);
	}
private:
	void* handle_ = nullptr; 
	Frame& frame_;
	MouseEventHandler* event_handler_ = nullptr;
	std::thread render_thread_;
	std::atomic_bool closed_ = false;
private:
	mutable std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
};