#pragma once

#include "luna.h"

enum class log_type { txt, err };

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
	void on_key_down(int key);
	void on_key_up(int key);

	void set_smoothing_mode(int v) { if (m_graphics) m_graphics->SetSmoothingMode((Gdiplus::SmoothingMode )v); }
	int set_back_color(lua_State* L);
	int set_line_color(lua_State* L);
	int set_fill_color(lua_State* L);
	void set_line_width(float w) { m_pen->SetWidth(w);}
	void set_start_cap(int v) { m_pen->SetStartCap((Gdiplus::LineCap)v);}
	void set_end_cap(int v) { m_pen->SetEndCap((Gdiplus::LineCap)v);}
	void set_dash_cap(int v) { m_pen->SetDashCap((Gdiplus::DashCap)v);}
	void set_dash_style(int v) { m_pen->SetDashStyle((Gdiplus::DashStyle)v); }
	void set_dash_offset(float v) { m_pen->SetDashOffset(v); }
	int draw_line(lua_State* L);
	int draw_polygon(lua_State* L);
	int fill_polygon(lua_State* L);
	void draw_ellipse(float x, float y, float w, float h);
	void fill_ellipse(float x, float y, float w, float h);
	void log_txt(const char* text) { log_split(text, log_type::txt); }
	void log_err(const char* text) { log_split(text, log_type::err); }
	void set_text_box(float x, float y, float w, float h);
	int set_log_back_color(lua_State* L) { m_log_back_color = get_color(L, 1); return 0; }
	int set_log_edge_color(lua_State* L) { m_log_edge_color = get_color(L, 1); return 0; }
	int set_log_err_color(lua_State* L) { m_log_err_color = get_color(L, 1); return 0; }
	int set_log_txt_color(lua_State* L) { m_log_txt_color = get_color(L, 1); return 0; }
	void log_clear() { m_lines.clear(); }

	void __gc() {}

	DECLARE_LUA_CLASS(winbox);

private:
	void load_script();
	void draw();
	void draw_text_lines();
	void log_split(const char* text, log_type tp);

	Gdiplus::PointF get_point(lua_State* L, int idx);
	Gdiplus::Color get_color(lua_State* L, int idx);

	template <typename T> void window_to_logic(T& x, T& y) { y = (T)m_height - y - (T)1; };
	template <typename T> void logic_to_window(T& x, T& y) { y = (T)m_height - y - (T)1; };

	HWND m_hwnd = nullptr;
	lua_State* m_lvm = nullptr;
	float m_width = 0;
	float m_height = 0;
	Gdiplus::Graphics* m_graphics = nullptr;
	Gdiplus::Color m_back_color;
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
