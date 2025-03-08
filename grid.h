#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include "life.h"
#include <vector>

class Grid {
    std::vector<std::vector<Cell>> m_Cells;
public:
    Grid();
    
    void Create(int row, int column);
    void Draw();  // Will be modified for OpenGL
    void Randomize();
    void Update(const Grid& next);
    
    bool WillSurvive(int row, int column) const;
    bool WillCreate(int row, int column) const;
    
    // New methods for 3D access
	// Add const qualifiers to accessors
	int GetWidth() const { return COL_MAX; }
	int GetHeight() const { return ROW_MAX; }
	const Cell& GetCell(int row, int col) const;
};

void Calculate(const Grid& oldGeneration, Grid& newGeneration);

#endif // GRID_H