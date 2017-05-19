#pragma once

#include "luna.h"

class winbox final
{
public:
	winbox(HWND hwnd);
	~winbox();

	bool setup();

	void on_paint();
	void on_size(int w, int h);
	void on_timer();
	void on_lbutton_up(int x, int y);
	void on_rbutton_up(int x, int y);
	void on_mouse_move(int x, int y);
	void on_key_down(int key);
	void on_key_up(int key);

	int set_back_color(lua_State* L);
	int set_line_color(lua_State* L);
	int set_fill_color(lua_State* L);
	int set_line_width(lua_State* L);
	int draw_line(lua_State* L);
	int draw_polygon(lua_State* L);
	int fill_polygon(lua_State* L);
	void draw_ellipse(float x, float y, float w, float h);
	void fill_ellipse(float x, float y, float w, float h);

	void __gc() {}

	DECLARE_LUA_CLASS(winbox);

private:
	void load_script();
	void draw();
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
	std::string m_msg;
};

extern winbox* g_app;
