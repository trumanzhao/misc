#pragma once

#include "cell_mgr.h"

enum class log_type { txt, err };
const int cell_size = 10;
const int padding = 10;

class winbox final
{
public:
	winbox();
	~winbox();

	void on_create(HWND hwnd);
	void on_paint();
	void on_size(int w, int h);
	void on_timer();
	void on_lbutton_up(int x, int y);
	void on_rbutton_up(int x, int y);
	void on_mouse_move(int x, int y);
	void on_key_down(WPARAM key);
	void on_key_up(WPARAM key);

	void draw_path(const std::vector<Gdiplus::PointF>& points);
	void draw_lines(const std::vector<Gdiplus::PointF>& points);
	void draw_polygon(const std::vector<Gdiplus::PointF>& points);
	void fill_polygon(const std::vector<Gdiplus::PointF>& points);
	void draw_ellipse(float x, float y, float w, float h);
	void fill_ellipse(float x, float y, float w, float h);
	void log_txt(const char* text) { log_split(text, log_type::txt); }
	void log_err(const char* text) { log_split(text, log_type::err); }
	void set_text_box(float x, float y, float w, float h);
	void log_clear() { m_lines.clear(); }

private:
	void draw();
	void draw_text_lines();
	void log_split(const char* text, log_type tp);
	cell_box* get_cell_by_point(int x, int y);

	HWND m_hwnd = nullptr;
	bool m_freeze = false;
	int m_frame = 0;
	float m_width = 0;
	float m_height = 0;
	cell_mgr m_cell_mgr;
	std::vector<Gdiplus::PointF> m_grid_lines;

	Gdiplus::Graphics* m_graphics = nullptr;
	Gdiplus::Color m_back_color = 0xff000000;
	Gdiplus::Pen* m_pen = nullptr;
	Gdiplus::SolidBrush* m_brush = nullptr;
	time_t m_check_time = 0;
	time_t m_entry_time = 0;

	Gdiplus::RectF m_log_box;
	Gdiplus::Color m_log_back_color = 0x33ffffff;
	Gdiplus::Color m_log_edge_color = 0xff00cc00;
	Gdiplus::Color m_log_err_color = 0xffff0000;
	Gdiplus::Color m_log_txt_color = 0xffffffff;
	struct log_line {std::wstring ws; log_type tp;};
	std::list<log_line> m_lines;
};

extern winbox* g_app;
