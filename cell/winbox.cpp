#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <vector>
#include <list>
#include <codecvt>
#include "winbox.h"

winbox* g_app = nullptr;

winbox::winbox()
{
	Gdiplus::Color pen_color(Gdiplus::Color::White);
	Gdiplus::Color brush_color(Gdiplus::Color::White);
	m_pen = new Gdiplus::Pen(pen_color);
	m_brush = new Gdiplus::SolidBrush(brush_color);
}

winbox::~winbox()
{
	delete m_brush;
	delete m_pen;
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

	int xcount = (w - padding * 2) / cell_size;
	int ycount = (h - padding * 2) / cell_size;

	m_cell_mgr.reset(xcount, ycount);

	m_grid_lines.clear();

	for (int x = 0; x <= xcount; x++)
	{
		m_grid_lines.push_back({(Gdiplus::REAL)padding + cell_size * x, (Gdiplus::REAL)padding});
		m_grid_lines.push_back({(Gdiplus::REAL)padding + cell_size * x, (Gdiplus::REAL)padding + cell_size * ycount});
	}

	for (int y = 0; y <= ycount; y++)
	{
		m_grid_lines.push_back({(Gdiplus::REAL)padding, (Gdiplus::REAL)padding + cell_size * y});
		m_grid_lines.push_back({(Gdiplus::REAL)padding + cell_size * xcount, (Gdiplus::REAL)padding + cell_size * y});
	}
}

void winbox::on_timer()
{
	m_frame++;
	if (!m_freeze)
	{
		m_cell_mgr.update();
	}
	InvalidateRect(m_hwnd, nullptr, false);	
}

void winbox::on_lbutton_up(int x, int y)
{
	auto* cell = get_cell_by_point(x, y);
	if (cell)
	{
		cell->alive = true;
	}
}

void winbox::on_rbutton_up(int x, int y)
{
	int rand_max = cell_size * 8;
	for (int i = 0; i < 16; i++)
	{
		int dx = rand() % (rand_max * 2) - rand_max;
		int dy = rand() % (rand_max * 2) - rand_max;
		auto* cell = get_cell_by_point(x + dx, y + dy);
		if (cell)
		{
			cell->alive = true;
		}
	}
}

void winbox::on_mouse_move(int x, int y)
{
}

void winbox::on_key_down(WPARAM key)
{
}

void winbox::on_key_up(WPARAM key)
{
	if (key == VK_ESCAPE)
	{
		m_freeze = !m_freeze;
		InvalidateRect(m_hwnd, nullptr, false);	
	}
}

void winbox::draw()
{
	m_graphics->Clear(m_back_color);

	draw_text_lines();

	m_pen->SetWidth(1.0f);
	m_pen->SetColor(m_freeze ? 0xff666600 : 0xff666666);
	draw_lines(m_grid_lines);

	int xcount = m_cell_mgr.get_width();
	int ycount = m_cell_mgr.get_height();
	std::vector<Gdiplus::PointF> rect;

	rect.resize(4);
	m_brush->SetColor(0xff009900);

	for (int y = 0; y < ycount; y++)
	{
		for (int x = 0; x < xcount; x++)
		{
			auto& cell = m_cell_mgr.get_cell(x, y);
			if (cell.alive)
			{
				rect[0] = {(Gdiplus::REAL)padding + cell_size * x + 2, (Gdiplus::REAL)padding + cell_size * y + 2};
				rect[1] = {(Gdiplus::REAL)padding + cell_size * x + 2, (Gdiplus::REAL)padding + cell_size * (y + 1) - 1};
				rect[2] = {(Gdiplus::REAL)padding + cell_size * (x + 1) - 1, (Gdiplus::REAL)padding + cell_size * (y + 1) - 1};
				rect[3] = {(Gdiplus::REAL)padding + cell_size * (x + 1) - 1, (Gdiplus::REAL)padding + cell_size * y + 2};
				fill_polygon(rect);
			}
		}
	}
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
	m_graphics->SetClip(m_log_box);
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
	m_graphics->ResetClip();

	Gdiplus::Pen pen(m_log_edge_color);
	m_graphics->DrawRectangle(&pen, m_log_box);
}

void winbox::draw_path(const std::vector<Gdiplus::PointF>& points)
{
	if (m_graphics == nullptr)
		return;

	int count = (int)points.size();
	for (int i = 1; i < count; i++)
	{
		m_graphics->DrawLine(m_pen, points[i - 1], points[i]);
	}
}

void winbox::draw_lines(const std::vector<Gdiplus::PointF>& points)
{
	if (m_graphics == nullptr)
		return;

	int count = (int)points.size();
	for (int i = 1; i < count; i += 2)
	{
		m_graphics->DrawLine(m_pen, points[i - 1], points[i]);
	}
}

void winbox::draw_polygon(const std::vector<Gdiplus::PointF>& points)
{
	int count = (int)points.size();
	if (m_graphics == nullptr || count < 3)
		return;

	m_graphics->DrawPolygon(m_pen, &points[0], count);
}

void winbox::fill_polygon(const std::vector<Gdiplus::PointF>& points)
{
	int count = (int)points.size();
	if (m_graphics == nullptr || count < 3)
		return;

	m_graphics->FillPolygon(m_brush, &points[0], (int)points.size());
}

void winbox::draw_ellipse(float x, float y, float w, float h)
{
	if (m_graphics == nullptr || w <= 2.0f || h <= 2.0f)
		return;
	m_graphics->DrawEllipse(m_pen, x, y, w, h);
}

void winbox::fill_ellipse(float x, float y, float w, float h)
{
	if (m_graphics == nullptr || w <= 2.0f || h <= 2.0f)
		return;
	m_graphics->FillEllipse(m_brush, x, y, w, h);
}

void winbox::set_text_box(float x, float y, float w, float h)
{
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

cell_box* winbox::get_cell_by_point(int x, int y)
{
	if (x < padding || y < padding)
		return nullptr;

	x = (x - padding) / cell_size;
	y = (y - padding) / cell_size;

	if (x >= m_cell_mgr.get_width() || y >= m_cell_mgr.get_height())
		return nullptr;

	return &m_cell_mgr.get_cell(x, y);
}
