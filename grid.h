#ifndef GRID_H
#define GRID_H

#include "cell.h"

#include <string>
#include <vector>
#include <cstdlib>


class Grid
{
	Cell m_Cells[ROW_MAX + 2][COL_MAX + 2];
public:
	void Create(int row, int column);
	void Draw();
	void Randomize();

	bool WillSurvive(int row, int column);
	bool WillCreate(int row, int column);

	void Update(const Grid& next);
};

void Calculate(Grid& oldGeneration, Grid& nextGeneration);

#endif //GRID_H_