#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <vector>
#include <list>
#include <codecvt>
#include "winbox.h"

winbox* g_app = nullptr;
static const char* g_entry = "main.lua";

EXPORT_CLASS_BEGIN(winbox)
EXPORT_LUA_FUNCTION(set_back_color)
EXPORT_LUA_FUNCTION(set_line_color)
EXPORT_LUA_FUNCTION(set_back_color)
EXPORT_LUA_FUNCTION(set_fill_color)
EXPORT_LUA_FUNCTION(set_line_width)
EXPORT_LUA_FUNCTION(draw_line)
EXPORT_LUA_FUNCTION(draw_polygon)
EXPORT_LUA_FUNCTION(fill_polygon)
EXPORT_LUA_FUNCTION(draw_ellipse)
EXPORT_LUA_FUNCTION(fill_ellipse)
EXPORT_LUA_STD_STR_AS(m_msg, "msg")
EXPORT_CLASS_END()

time_t get_file_time(const char* file_name)
{
	if (file_name == nullptr)
		return 0;

	struct stat file_info;
	int ret = stat(file_name, &file_info);
	if (ret != 0)
		return 0;
	return file_info.st_mtime;
}

winbox::winbox(HWND hwnd) : m_hwnd(hwnd)
{
	Gdiplus::Color pen_color(Gdiplus::Color::White);
	Gdiplus::Color brush_color(Gdiplus::Color::White);
	m_lvm = luaL_newstate();
	luaL_openlibs(m_lvm);
	m_pen = new Gdiplus::Pen(pen_color);
	m_brush = new Gdiplus::SolidBrush(brush_color);
}

winbox::~winbox()
{
	delete m_brush;
	delete m_pen;
	lua_close(m_lvm);
}

bool winbox::setup()
{
	luaL_openlibs(m_lvm);
	lua_push_object(m_lvm, this);
	lua_setglobal(m_lvm, "window");
	load_script();
	return true;
}

void winbox::on_paint()
{
	if (m_width < 100 || m_height < 100)
		return;

	Gdiplus::Bitmap bmp((int)m_width, (int)m_height);
	Gdiplus::Graphics graphics(&bmp);

	m_graphics = &graphics;
	draw();
	m_graphics = nullptr;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hwnd, &ps);
	if (hdc != nullptr)
	{
		Gdiplus::Graphics win_graphic(hdc);
		win_graphic.DrawImage(&bmp, 0, 0);
		EndPaint(m_hwnd, &ps);
	}
}

void winbox::on_size(int w, int h)
{
	m_width = (float)w;
	m_height = (float)h;

	lua_guard g(m_lvm);
	lua_call_object_function(m_lvm, this, "on_size", std::tie(), w, h);
}

void winbox::on_timer()
{
	time_t filetime = get_file_time(g_entry);
	if (filetime != m_entry_time)
	{
		m_entry_time = filetime;
		load_script();
	}

	InvalidateRect(m_hwnd, nullptr, false);
	lua_guard g(m_lvm);
	lua_call_object_function(m_lvm, this, "on_timer");
}

void winbox::on_lbutton_up(int x, int y)
{
	lua_guard g(m_lvm);
	window_to_logic(x, y);
	lua_call_object_function(m_lvm, this, "on_lbutton_up", std::tie(), x, y);
}

void winbox::on_rbutton_up(int x, int y)
{
	lua_guard g(m_lvm);
	window_to_logic(x, y);
	lua_call_object_function(m_lvm, this, "on_rbutton_up", std::tie(), x, y);
}

void winbox::on_mouse_move(int x, int y)
{
	lua_guard g(m_lvm);
	window_to_logic(x, y);
	lua_call_object_function(m_lvm, this, "on_mouse_move", std::tie(), x, y);
}

void winbox::on_key_down(int key)
{
	lua_guard g(m_lvm);
	lua_call_object_function(m_lvm, this, "on_key_down", std::tie(), key);
}

void winbox::on_key_up(int key)
{
	lua_guard g(m_lvm);
	lua_call_object_function(m_lvm, this, "on_key_up", std::tie(), key);
}

void winbox::load_script()
{
	m_entry_time  = get_file_time(g_entry);

	m_msg = "";
	if (luaL_dofile(m_lvm, g_entry) != 0)
	{
		const char* err = lua_tostring(m_lvm, -1);
		if (err)
		{
			m_msg = err;
		}
	}
}

void winbox::draw()
{
	m_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	m_graphics->Clear(m_back_color);

	lua_guard g(m_lvm);
	lua_call_object_function(m_lvm, this, "on_draw");

	if (m_msg.size() > 0)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		std::wstring ws = conv.from_bytes(m_msg);
		Gdiplus::Font font(L"Arial", 16);
		Gdiplus::RectF rect(0, 0, m_width, m_height);
		Gdiplus::StringFormat format;
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));

		format.SetAlignment(Gdiplus::StringAlignmentNear);
		m_graphics->DrawString(ws.c_str(), (int)ws.size(), &font, rect, &format, &brush);
	}
}

Gdiplus::PointF winbox::get_point(lua_State* L, int idx)
{
	lua_guard g(L);

	lua_getfield(L, idx, "x");
	float x = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, idx, "y");
	float y = (float)lua_tonumber(L, -1);

	logic_to_window(x, y);

	return Gdiplus::PointF(x, m_height - y - 1);
}

Gdiplus::Color winbox::get_color(lua_State* L, int idx)
{
	lua_guard lg(L);

	int tp = lua_type(L, idx);
	if (tp == LUA_TNUMBER)
	{
		return Gdiplus::Color((uint32_t)lua_tonumber(L, idx));
	}

	if (tp != LUA_TTABLE)
	{
		return Gdiplus::Color(255, 0, 0);
	}

	lua_getfield(L, idx, "r");
	BYTE r = (BYTE)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, idx, "g");
	BYTE g = (BYTE)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, idx, "b");
	BYTE b = (BYTE)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, idx, "b");
	if (lua_isnil(L, -1))
	{
		return Gdiplus::Color(r, g, b);
	}
	BYTE a = (BYTE)lua_tonumber(L, -1);
	return Gdiplus::Color(r, g, b, a);
}

int winbox::set_back_color(lua_State* L)
{
	m_back_color = get_color(L, 1);
	return 0;
}

int winbox::set_line_color(lua_State* L)
{
	m_pen->SetColor(get_color(L, 1));
	return 0;
}

int winbox::set_fill_color(lua_State* L)
{
	m_brush->SetColor(get_color(L, 1));
	return 0;
}

int winbox::set_line_width(lua_State* L)
{
	m_pen->SetWidth((float)lua_tonumber(L, 1));
	return 0;
}

int winbox::draw_line(lua_State* L)
{
	int count = lua_gettop(L);

	if (m_graphics == nullptr || count < 2)
		return 0;

	Gdiplus::PointF last;
	for (int i = 1; i <= count; i++)
	{
		Gdiplus::PointF point = get_point(L, i);

		if (i > 1)
		{
			m_graphics->DrawLine(m_pen, last, point);
		}
		last = point;
	}

	return 0;
}

int winbox::draw_polygon(lua_State* L)
{
	std::vector<Gdiplus::PointF> points;
	int count = lua_gettop(L);

	if (m_graphics == nullptr || count != 1)
		return 0;

	int len = (int)luaL_len(L, 1);
	if (len < 3)
	{
		return 0;
	}

	for (int i = 1; i <= len; i++)
	{
		lua_pushinteger(L, i);
		lua_gettable(L, 1);
		points.push_back(get_point(L, -1));
		lua_pop(L, 1);
	}

	m_graphics->DrawPolygon(m_pen, &points[0], (int)points.size());
	return 0;
}

int winbox::fill_polygon(lua_State* L)
{
	std::vector<Gdiplus::PointF> points;
	int count = lua_gettop(L);

	if (m_graphics == nullptr || count != 1)
		return 0;

	int len = (int)luaL_len(L, 1);
	if (len < 3)
	{
		return 0;
	}

	for (int i = 1; i <= len; i++)
	{
		lua_pushinteger(L, i);
		lua_gettable(L, 1);
		points.push_back(get_point(L, -1));
		lua_pop(L, 1);
	}

	m_graphics->FillPolygon(m_brush, &points[0], (int)points.size());
	return 0;
}

void winbox::draw_ellipse(float x, float y, float w, float h)
{
	if (m_graphics == nullptr || w <= 2.0f || h <= 2.0f)
		return;
	logic_to_window(x, y);
	m_graphics->DrawEllipse(m_pen, x, y, w, h);
}

void winbox::fill_ellipse(float x, float y, float w, float h)
{
	if (m_graphics == nullptr || w <= 2.0f || h <= 2.0f)
		return;
	logic_to_window(x, y);
	m_graphics->FillEllipse(m_brush, x, y, w, h);
}
