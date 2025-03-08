#pragma once
#ifndef LIFE_H
#define LIFE_H

// Grid dimensions (reduce for better 3D performance)
const int ROW_MAX = 40;
const int COL_MAX = 40;

// Conway's Game rules
const int MIN_NEIGHBOURS = 2;
const int MAX_NEIGHBOURS = 3;
const int MIN_PARENTS = 3;
const int MAX_PARENTS = 3;

// 3D cell properties
const float CELL_SIZE = 0.8f;
const float CELL_SPACING = 1.0f;

#endif // LIFE_H