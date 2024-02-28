#include "cell.h"

void Cell::Draw(int row, int col) const
{
	//escape[n;mH moves the cursor to row n, column m 
	std::cout << "\x1b[" << row + 1 << ";" << col + 1 << "H";
	std::cout << (m_Alive ? LIVE_CELL : DEAD_CELL);
}