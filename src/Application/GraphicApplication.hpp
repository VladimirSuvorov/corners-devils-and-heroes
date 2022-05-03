#pragma once
#include "../Graphics/QuadBatchMeshRenderer.hpp"
#include "../Window/RenderWindow.hpp"
#include "GraphicApplicationRenderFrame.hpp"

class GraphicApplication {
public:
	using window_t = RenderWindow;
	GraphicApplication(uint32_t window_width, uint32_t window_height, const char* window_title, GameView& view)noexcept;
	//run - ����������� ���������� �����, �������� ���� ������� ����
	void run()noexcept;
	virtual ~GraphicApplication();
protected: // ����������������� ������� - ����������� ����������
	window_t& get_main_window() noexcept {
		return main_window_;
	}

	//���������� ������ (��� ������ ����� is_running_ == false, ����� ������������)
	void close()noexcept;
	//��� ������, ���� ���� �������� ��������� �� ������ ������� (��������)
	virtual void OnWindowTriesToClose() = 0;
	//��� ������, ��� ��������� ��������� ���� �� ��
	virtual void OnWindowUpdate() = 0;
	//��� ������ ��� ���������� ������
	virtual void OnTick() = 0;

protected:
	GraphicApplicationRenderFrame frame_;
private:
	//render � update - �������� � ��������� ������� - render_thread_ � update_thread_ ��������������; �������� �� ������� � ������ ��������������.
	void threaded_update()noexcept;
private:
	std::atomic_bool is_running_;

	std::atomic_bool update_thread_finished_;
	std::thread update_thread_;
	window_t main_window_;
private:
	std::chrono::milliseconds update_cooldown_ = 10ms,	elapsed_since_last_update_ = 0ms;
	std::chrono::high_resolution_clock::time_point last_update_timestamp_ = std::chrono::high_resolution_clock::now();
};


