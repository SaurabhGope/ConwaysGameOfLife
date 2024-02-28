#pragma once
#ifndef CELL_H
#define CELL_H

#include <iostream>
#include "life.h"

class Cell
{
	private:
		bool m_Alive;
	public:
		// Cells are empty by default
		Cell() : m_Alive(false) {}

		void Draw(int row, int col) const;
		void Create() { m_Alive = true; }
		void Erase() { m_Alive = false;}
		bool IsAlive() const { return m_Alive; }
};

#endif //CELL_H