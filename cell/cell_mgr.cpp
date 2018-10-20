#include "cell_mgr.h"

void cell_mgr::update()
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			update_cell(x, y);			
		}
	}
	m_cells.swap(m_back);
}

void cell_mgr::update_cell(int x, int y)
{
	auto& cell = get_cell(x, y);
	auto& back = get_back(x, y);
	int neighbor_count = 0;

	neighbor_count += get_cell(x - 1, y - 1).alive ? 1 : 0;
	neighbor_count += get_cell(x - 1, y).alive ? 1 : 0;
	neighbor_count += get_cell(x - 1, y + 1).alive ? 1 : 0;
	neighbor_count += get_cell(x, y + 1).alive ? 1 : 0;

	neighbor_count += get_cell(x + 1, y + 1).alive ? 1 : 0;
	neighbor_count += get_cell(x + 1, y).alive ? 1 : 0;
	neighbor_count += get_cell(x + 1, y - 1).alive ? 1 : 0;
	neighbor_count += get_cell(x, y - 1).alive ? 1 : 0;

	/*
	1． 如果一个细胞周围有3个细胞为生（一个细胞周围共有8个细胞），则该细胞为生（即该细胞若原先为死，则转为生，若原先为生，则保持不变） 。
	2． 如果一个细胞周围有2个细胞为生，则该细胞的生死状态保持不变；
	3． 在其它情况下，该细胞为死（即该细胞若原先为生，则转为死，若原先为死，则保持不变）
	*/

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
