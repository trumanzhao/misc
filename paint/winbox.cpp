#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <vector>
#include <list>
#include <codecvt>
#include "winbox.h"

winbox* g_app = nullptr;
static const char* g_entry = "hive.lua";

EXPORT_CLASS_BEGIN(winbox)
EXPORT_LUA_FUNCTION(set_smoothing_mode)
EXPORT_LUA_FUNCTION(set_back_color)
EXPORT_LUA_FUNCTION(set_line_color)
EXPORT_LUA_FUNCTION(set_fill_color)
EXPORT_LUA_FUNCTION(set_line_width)
EXPORT_LUA_FUNCTION(set_start_cap)
EXPORT_LUA_FUNCTION(set_end_cap)
EXPORT_LUA_FUNCTION(set_dash_cap)
EXPORT_LUA_FUNCTION(set_dash_style)
EXPORT_LUA_FUNCTION(set_dash_offset)
EXPORT_LUA_FUNCTION(draw_line)
EXPORT_LUA_FUNCTION(draw_polygon)
EXPORT_LUA_FUNCTION(fill_polygon)
EXPORT_LUA_FUNCTION(draw_ellipse)
EXPORT_LUA_FUNCTION(fill_ellipse)
EXPORT_LUA_FUNCTION(log_err)
EXPORT_LUA_FUNCTION(log_txt)
EXPORT_LUA_FUNCTION(set_text_box)
EXPORT_LUA_FUNCTION(set_log_back_color)
EXPORT_LUA_FUNCTION(set_log_edge_color)
EXPORT_LUA_FUNCTION(set_log_err_color)
EXPORT_LUA_FUNCTION(set_log_txt_color)
EXPORT_LUA_FUNCTION(log_clear)
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

winbox::winbox()
{
	Gdiplus::Color pen_color(Gdiplus::Color::White);
	Gdiplus::Color brush_color(Gdiplus::Color::White);
	m_lvm = luaL_newstate();
	m_pen = new Gdiplus::Pen(pen_color);
	m_brush = new Gdiplus::SolidBrush(brush_color);
}

winbox::~winbox()
{
	delete m_brush;
	delete m_pen;
	lua_close(m_lvm);
}

void winbox::on_create(HWND hwnd)
{
	RECT rect;

	m_hwnd = hwnd;
	GetClientRect(m_hwnd, &rect);
	m_width = (float)rect.right;
	m_height =(float)rect.bottom;

	float x = (float)(rect.right / 4), y = (float)(rect.bottom / 4);
	float w = (float)(rect.right / 2), h = (float)(rect.bottom / 2);
	m_log_box = Gdiplus::RectF(x, y, w, h);

	luaL_openlibs(m_lvm);
	lua_push_object(m_lvm, this);
	lua_setglobal(m_lvm, "window");
	load_script();
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

	std::string err;
	lua_guard g(m_lvm);
	lua_call_object_function(err, m_lvm, this, "on_size", std::tie(), w, h);
	log_err(err.c_str());
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

	std::string err;
	lua_guard g(m_lvm);
	lua_call_object_function(err, m_lvm, this, "on_timer");
	log_err(err.c_str());
}

void winbox::on_lbutton_up(int x, int y)
{
	std::string err;
	lua_guard g(m_lvm);
	window_to_logic(x, y);
	lua_call_object_function(err, m_lvm, this, "on_lbutton_up", std::tie(), x, y);
	log_err(err.c_str());
}

void winbox::on_rbutton_up(int x, int y)
{
	std::string err;
	lua_guard g(m_lvm);
	window_to_logic(x, y);
	lua_call_object_function(err, m_lvm, this, "on_rbutton_up", std::tie(), x, y);
	log_err(err.c_str());
}

void winbox::on_mouse_move(int x, int y)
{
	std::string err;
	lua_guard g(m_lvm);
	window_to_logic(x, y);
	lua_call_object_function(err, m_lvm, this, "on_mouse_move", std::tie(), x, y);
	log_err(err.c_str());
}

void winbox::on_key_down(int key)
{
	std::string err;
	lua_guard g(m_lvm);
	lua_call_object_function(err, m_lvm, this, "on_key_down", std::tie(), key);
	log_err(err.c_str());
}

void winbox::on_key_up(int key)
{
	std::string err;
	lua_guard g(m_lvm);
	lua_call_object_function(err, m_lvm, this, "on_key_up", std::tie(), key);
	log_err(err.c_str());
}

void winbox::load_script()
{
	m_entry_time  = get_file_time(g_entry);

	if (luaL_dofile(m_lvm, g_entry) != 0)
	{
		const char* err = lua_tostring(m_lvm, -1);
		if (err)
		{
			log_err(err);
		}
	}
}

void winbox::draw()
{
	m_graphics->Clear(m_back_color);

	std::string err;
	lua_guard g(m_lvm);
	lua_call_object_function(err, m_lvm, this, "on_draw");
	log_err(err.c_str());
	draw_text_lines();
}

void winbox::draw_text_lines()
{
	if (m_lines.empty())
		return;

	Gdiplus::SolidBrush brush(m_log_back_color);
	m_graphics->FillRectangle(&brush, m_log_box);

	Gdiplus::Font font(L"宋体", 12);
	Gdiplus::StringFormat format;
	
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	format.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);

	float line_pos = m_log_box.GetBottom();
	float fontHeight = font.GetHeight(m_graphics) + 3;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		if (line_pos < m_log_box.Y)
		{
			m_lines.erase(it, m_lines.end());
			break;
		}
		Gdiplus::RectF rect(m_log_box.X, line_pos - fontHeight, m_log_box.Width, fontHeight);
		brush.SetColor(it->tp == log_type::err ? m_log_err_color : m_log_txt_color);
		m_graphics->DrawString(it->ws.c_str(), (int)it->ws.size(), &font, rect, &format, &brush);
		line_pos -= fontHeight;
	}

	Gdiplus::Pen pen(m_log_edge_color);
	m_graphics->DrawRectangle(&pen, m_log_box);
}

// 支持{x=1,y=2}格式,也支持{x,y}格式
Gdiplus::PointF winbox::get_point(lua_State* L, int idx)
{
	lua_guard g(L);
	float x= 0, y = 0;

	if (luaL_len(L, idx) == 2)
	{
		lua_geti(L, idx, 1);
		x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_geti(L, idx, 2);
		x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		lua_getfield(L, idx, "x");
		x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, "y");
		y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}

	logic_to_window(x, y);

	return Gdiplus::PointF(x, m_height - y - 1);
}

Gdiplus::Color winbox::get_color(lua_State* L, int idx)
{
	lua_guard lg(L);
	BYTE a=255, r=255, g = 0, b = 0;

	int tp = lua_type(L, idx);
	if (tp == LUA_TNUMBER)
	{
		return Gdiplus::Color((uint32_t)lua_tonumber(L, idx));
	}

	if (tp != LUA_TTABLE)
	{
		return Gdiplus::Color(r, g, b);
	}

	int len = (int)luaL_len(L, idx);
	if (len == 3)
	{
		lua_geti(L, idx, 1);
		r = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_geti(L, idx, 2);
		g = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_geti(L, idx, 3);
		b = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	else if (len == 4)
	{
		lua_geti(L, idx, 1);
		a = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_geti(L, idx, 2);
		r = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_geti(L, idx, 3);
		g = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_geti(L, idx, 4);
		b = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	else
	{
		lua_getfield(L, idx, "r");
		r = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, "g");
		g = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, "b");
		b = (BYTE)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, idx, "a");
		if (!lua_isnil(L, -1))
		{
			a = (BYTE)lua_tonumber(L, -1);
		}
		lua_pop(L, 1);
	}
	return Gdiplus::Color(a, r, g, b);
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

void winbox::set_text_box(float x, float y, float w, float h)
{
	logic_to_window(x, y);
	m_log_box = {x, y, w, h};
}

void winbox::log_split(const char* text, log_type tp)
{
	if (text == nullptr || *text == '\0')
		return;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring ws = conv.from_bytes(text);
	std::wstring::size_type prev_pos = 0, pos = 0;
    do
    {
		pos = ws.find(L"\n", pos);
		if (pos == std::wstring::npos)
		{
			m_lines.push_front({ws.substr(prev_pos), tp});
			break;
		}
		else
		{
			m_lines.push_front({ws.substr(prev_pos, pos - prev_pos), tp});
			prev_pos = ++pos;
		}
	} while (true);
}
