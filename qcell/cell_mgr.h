#pragma once

#include <vector>

struct CellBox
{
	bool alive;
};

class CellMgr final
{
public:
    CellMgr() {}
    ~CellMgr() {}

	void reset(int w, int h) 
	{
        m_Width = w;
        m_Height = h;
        m_Cells.clear();

        auto size = static_cast<size_t>(m_Width * m_Height);

        m_Cells.resize(size, {false});
        m_Back.clear();
        m_Back.resize(size, {false});
	}

    int getWidth(){ return m_Width; }
    int getHeight(){ return m_Height; }

    CellBox& getCell(int x, int y)
	{
		static_assert((-7) % 3 == -1, "check your compiler");
        x = ((x % m_Width) + m_Width) % m_Width;
        y = ((y % m_Height) + m_Height) % m_Height;
        return m_Cells[static_cast<size_t>(m_Width * y + x)];
	}

    CellBox& getBack(int x, int y)
	{
		static_assert((-7) % 3 == -1, "check your compiler");
        x = ((x % m_Width) + m_Width) % m_Width;
        y = ((y % m_Height) + m_Height) % m_Height;
        return m_Back[static_cast<size_t>(m_Width * y + x)];
	}

	void update();

private:

	void update_cell(int x, int y);

private:
    int m_Width = 0;
    int m_Height = 0;
    std::vector<CellBox> m_Cells;
    std::vector<CellBox> m_Back;
};
