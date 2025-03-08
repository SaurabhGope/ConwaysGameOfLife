#include "grid.h"
#include "grid.h"
#include <time.h>

Grid::Grid() : m_Cells(ROW_MAX, std::vector<Cell>(COL_MAX)) {} // Initialize with proper size

void Grid::Create(int row, int column) {
    // Add boundary check
    if(row >= 0 && row < ROW_MAX && column >= 0 && column < COL_MAX) {
        m_Cells[row][column].Create();
    }
}

const Cell& Grid::GetCell(int row, int col) const {
    if(row >= 0 && row < ROW_MAX && col >= 0 && col < COL_MAX) {
        return m_Cells[row][col];
    }
    static Cell nullCell;
    return nullCell;
}
void Grid::Randomize()
{
    srand(static_cast<unsigned>(time(nullptr)));
    const int factor = 5;
    const int cutOff = RAND_MAX / factor;

    for(int row = 0; row < ROW_MAX; ++row)
	{
        for(int column = 0; column < COL_MAX; ++column)
		{
            if(rand()/cutOff == 0)
			{
                Create(row, column);
            }
        }
    }
}

bool Grid::WillSurvive(int row, int column) const
{
	if(row <= 0 || row >= ROW_MAX-1 || column <= 0 || column >= COL_MAX-1) 
	{
		return false;
	}

	if (!m_Cells[row][column].IsAlive())
	{
		return false;
	}

	// Find the number of live neighbours for this cell
	//
	//   x x x
	//   x o x
	//   x x x
	int neighbours = m_Cells[row - 1][column - 1].IsAlive() +
		m_Cells[row - 1][column].IsAlive() +
		m_Cells[row - 1][column + 1].IsAlive() +
		m_Cells[row][column - 1].IsAlive() +
		m_Cells[row][column + 1].IsAlive() +
		m_Cells[row + 1][column - 1].IsAlive() +
		m_Cells[row + 1][column].IsAlive() +
		m_Cells[row + 1][column + 1].IsAlive();

	if (neighbours < MIN_NEIGHBOURS || neighbours > MAX_NEIGHBOURS)
	{
		// Cell has died
		return false;
	}

	return true;
}

bool Grid::WillCreate(int row, int column) const
{
	if(row <= 0 || row >= ROW_MAX-1 || column <= 0 || column >= COL_MAX-1) 
	{
		return false;
	}
	if (m_Cells[row][column].IsAlive())
	{
		return false;
	}

	// Find the number of parents for this cell
	//
	//   x x x
	//   x o x
	//   x x x
	int parents = m_Cells[row - 1][column - 1].IsAlive() +
		m_Cells[row - 1][column].IsAlive() +
		m_Cells[row - 1][column + 1].IsAlive() +
		m_Cells[row][column - 1].IsAlive() +
		m_Cells[row][column + 1].IsAlive() +
		m_Cells[row + 1][column - 1].IsAlive() +
		m_Cells[row + 1][column].IsAlive() +
		m_Cells[row + 1][column + 1].IsAlive();

	if (parents < MIN_PARENTS || parents > MAX_PARENTS)
	{
		return false;
	}
	
	return true;
}

void Grid::Update(const Grid& next) {
    for(int row = 0; row < ROW_MAX; row++) {  // Full range
        for(int column = 0; column < COL_MAX; column++) {
            if(row < next.m_Cells.size() && column < next.m_Cells[row].size()) {
                m_Cells[row][column] = next.m_Cells[row][column];
            }
        }
    }
}

void Calculate(const Grid& oldGeneration, Grid& newGeneration)
{
	newGeneration = Grid();
	for (int row = 0; row < ROW_MAX; ++row)
	{
		for (int column = 0; column < COL_MAX; ++column)
		{
			// will this live cell survive to next generation?
			if (oldGeneration.WillSurvive(row, column))
			{
				newGeneration.Create(row, column);
			}
			// will this unpopulated cell be populated in next generation?
			else if (oldGeneration.WillCreate(row, column))
			{
				newGeneration.Create(row, column);
			}
		}
	}
}
