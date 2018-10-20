#include "cell_mgr.h"

void CellMgr::update()
{
	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			update_cell(x, y);			
		}
	}
	m_Cells.swap(m_Back);
}

void CellMgr::update_cell(int x, int y)
{
    auto& cell = getCell(x, y);
    auto& back = getBack(x, y);
	int neighbor_count = 0;

    neighbor_count += getCell(x - 1, y - 1).alive ? 1 : 0;
    neighbor_count += getCell(x - 1, y).alive ? 1 : 0;
    neighbor_count += getCell(x - 1, y + 1).alive ? 1 : 0;
    neighbor_count += getCell(x, y + 1).alive ? 1 : 0;

    neighbor_count += getCell(x + 1, y + 1).alive ? 1 : 0;
    neighbor_count += getCell(x + 1, y).alive ? 1 : 0;
    neighbor_count += getCell(x + 1, y - 1).alive ? 1 : 0;
    neighbor_count += getCell(x, y - 1).alive ? 1 : 0;

	if (neighbor_count == 3)
	{
		back.alive = true;
	}
	else if (neighbor_count == 2)
	{
		back.alive = cell.alive;
	}
	else
	{
		back.alive = false;
	}
}
