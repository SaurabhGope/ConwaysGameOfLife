#include "grid.h"
#include "grid.h"
#include <time.h>

void Grid::Create(int row, int column)
{
	m_Cells[row][column].Create();
}
void Grid::Draw()
{
	// Escape[2J clears the screen and returns the cursor to the "home" position
	std::cout << "\x1b[2J";

	for (int row = 0; row < ROW_MAX; ++row)
	{
		for (int column = 0; column < COL_MAX; ++column)
		{
			m_Cells[row][column].Draw(row, column);
		}
	}
}

void Grid::Randomize()
{
	const int factor = 5;
	const int cutOff = RAND_MAX / factor;
	time_t now;
	time(&now);
	srand(now);

	for (int row = 1; row < ROW_MAX; ++row)
	{
		for (int column = 1; column < COL_MAX; ++column)
		{
			if (rand()/cutOff == 0)
			{
				Create(row, column);
			}
		}
	}
}

bool Grid::WillSurvive(int row, int column)
{
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

bool Grid::WillCreate(int row, int column)
{
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

void Grid::Update(const Grid& next)
{
	for (int row = 1; row < ROW_MAX; ++row)
	{
		for (int column = 1; column < COL_MAX; ++column)
		{
			m_Cells[row][column] = next.m_Cells[row][column];
		}
	}
}

void Calculate(Grid& oldGeneration, Grid& newGeneration)
{
	for (int row = 1; row < ROW_MAX; ++row)
	{
		for (int column = 1; column < COL_MAX; ++column)
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
