#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#include <GLFW/glfw3.h>
#include "../undef_gl_macros.h"
#include "RenderWindow.hpp"
#include "../utilities.h"


static bool IsGlfwInit = false;
bool InitGlfw() {
	if (!IsGlfwInit) [[unlikely]] { 
		glfwSetErrorCallback([](int error, const char* description) {
			LOG_INFO("Error (%d): %s\n", error, description);
		});
		if (IsGlfwInit = glfwInit()) {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
			//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
			//glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
			glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
			
			//glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

			glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
			glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
			glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
			glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
			// glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
			glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

			glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
			glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
			glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);

			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
			glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_NONE);
			glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
		}
	}
	return IsGlfwInit;
}
void TermGlfw() {
	if (IsGlfwInit) [[unlikely]] {
		IsGlfwInit = false;
		glfwTerminate();
	}
}
static uint32_t WindowCount = 0;


RenderWindow::RenderWindow(uint32_t width, uint32_t height, const char* title, Frame& frame) :
	frame_(frame)
{
	if (InitGlfw()) {
		WindowCount += static_cast<bool>(handle_ = glfwCreateWindow(width, height, title, nullptr, nullptr));
		Expects(handle_);
		auto window = static_cast<GLFWwindow*>(handle_);
		glfwSetWindowUserPointer(window, this);
		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {

			auto render_window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
			if (auto handler = render_window->get_mouse_event_handler())
				handler->OnButtonChangesState(button - GLFW_MOUSE_BUTTON_1, action == GLFW_PRESS);
		});
		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			auto render_window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
			if (auto handler = render_window->get_mouse_event_handler())
				handler->OnMouseMove(xpos, ypos);
		});
		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			auto render_window = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
			render_window->close();
		});
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		auto monitor = []()->GLFWmonitor*{
			int monitor_count;
			auto monitors = glfwGetMonitors(&monitor_count);
			return monitor_count > 1 ? monitors[1] : monitors[0];
		}();
		auto vm = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(window, (vm->width - w) / 2, (vm->height - h) / 2);
#if 0
		if (glfwGetPrimaryMonitor() != monitor) {
			glfwSetWindowOpacity(window, 0.0625);
		}
#endif
		render_thread_ = std::thread(&RenderWindow::threaded_render, this);
	}
}
RenderWindow::~RenderWindow() {
	close();
	if (render_thread_.joinable())
		render_thread_.join();
	if (WindowCount > 0) {
		glfwDestroyWindow(static_cast<GLFWwindow*>(handle_));
		if (!(--WindowCount))
			TermGlfw();
	}
}
void RenderWindow::threaded_render() {
	auto window = static_cast<GLFWwindow*>(handle_);
	glfwMakeContextCurrent(window);
	glbinding::initialize(glfwGetProcAddress); 
	gl::glDebugMessageCallback([](gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length, const gl::GLchar* message, const void* userParam)
		{
			const size_t Length = length;
			const char* Source = "UNDEFINDED";
			switch (source) {
			case gl::GL_DEBUG_SOURCE_API:
				Source = "API";
				break;
			case gl::GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				Source = "WINDOWN SYSTEM";
				break;
			case gl::GL_DEBUG_SOURCE_SHADER_COMPILER:
				Source = "SHADER COMPILER";
				break;
			case gl::GL_DEBUG_SOURCE_THIRD_PARTY:
				Source = "THIRD PARTY";
				break;
			case gl::GL_DEBUG_SOURCE_APPLICATION:
				Source = "APPLICATION";
				break;
			case gl::GL_DEBUG_SOURCE_OTHER:
				Source = "OTHER";
				break;
			default:
				Source = "UNKNOWN";
				break;
			}

			switch (severity) {
			case gl::GL_DEBUG_SEVERITY_HIGH:
				LOG_INFO("OpenGL Error at [%s]: %s", Source, message);
				break;
			case gl::GL_DEBUG_SEVERITY_MEDIUM:
				LOG_INFO("OpenGL Warning at [%s]: %s", Source, message);
				break;
			case gl::GL_DEBUG_SEVERITY_LOW:
				LOG_INFO("OpenGL Info at [%s]: %s", Source, message);
				break;
			case gl::GL_DEBUG_SEVERITY_NOTIFICATION:
				LOG_INFO("OpenGL Trace at [%s]: %s", Source, message);
				break;
			}
		}, nullptr);
	gl::glEnable(gl::GL_DEBUG_OUTPUT);
	gl::glEnable(gl::GL_DEBUG_OUTPUT_SYNCHRONOUS);
	gl::glEnable(gl::GL_BLEND);
	gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);


	std::chrono::milliseconds render_cooldown = 10ms, elapsed_since_last_render = 0ms;
	std::chrono::high_resolution_clock::time_point last_render_timestamp = std::chrono::high_resolution_clock::now();
	frame_.initialize_callback();
	while (!closed_) {
		if ((std::chrono::high_resolution_clock::now() - last_render_timestamp) > render_cooldown) {
			lock();
			frame_.render_callback();
			glfwSwapBuffers(window);
			unlock();
			last_render_timestamp = std::chrono::high_resolution_clock::now();
		} else
			std::this_thread::sleep_until(last_render_timestamp + render_cooldown - 50us);
	}
	frame_.dispose_callback();
}

void RenderWindow::close() {
	closed_ = true;
}
void RenderWindow::poll_events() {
	glfwPollEvents();
}

bool RenderWindow::should_close()const {
	return closed_;
}

bool RenderWindow::is_key_pressed(char32_t Key)const {
	if (!closed_) {
		bool pressed = false;
		switch (Key) {
		case 'ESC':
			pressed = glfwGetKey(static_cast<GLFWwindow*>(handle_), GLFW_KEY_ESCAPE);
			break;
		}
		return pressed;
	}
	return false;
}

std::pair<uint32_t, uint32_t> RenderWindow::size()const {
	if (!closed_) {
		int w, h;
		glfwGetWindowSize(static_cast<GLFWwindow*>(handle_), &w, &h);
		return { static_cast<uint32_t>(w), static_cast<uint32_t>(h) };
	}
	return { 0,0 };
}
