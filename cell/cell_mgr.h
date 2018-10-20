#pragma once

#include <vector>

struct cell_box
{
	bool alive;
};

class cell_mgr final
{
public:
	cell_mgr() {}
	~cell_mgr() {}

	void reset(int w, int h) 
	{
		m_width = w;
		m_height = h;
		m_cells.clear();
		m_cells.resize(m_width * m_height, {false});
		m_back.clear();
		m_back.resize(m_width * m_height, {false});
	}

	int get_width(){ return m_width; }
	int get_height(){ return m_height; }

	cell_box& get_cell(int x, int y)
	{
		static_assert((-7) % 3 == -1, "check your compiler");
		x = ((x % m_width) + m_width) % m_width;
		y = ((y % m_height) + m_height) % m_height;
		return m_cells[m_width * y + x];
	}

	cell_box& get_back(int x, int y)
	{
		static_assert((-7) % 3 == -1, "check your compiler");
		x = ((x % m_width) + m_width) % m_width;
		y = ((y % m_height) + m_height) % m_height;
		return m_back[m_width * y + x];
	}

	void update();

private:

	void update_cell(int x, int y);

private:
	int m_width = 0;
	int m_height = 0;
	std::vector<cell_box> m_cells;
	std::vector<cell_box> m_back;
};
