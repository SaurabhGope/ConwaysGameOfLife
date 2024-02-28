#pragma once

#ifndef LIFE_H
#define LIFE_H

const char LIVE_CELL{ 'X' };
const char DEAD_CELL{ ' ' };

//ANSI CONSOLE BORDER Length
const int ROW_MAX = 23;
const int COL_MAX = 79;

//Conway's Parameters
const int MIN_NEIGHBOURS = 2;
const int MAX_NEIGHBOURS = 3;
const int MIN_PARENTS = 3;
const int MAX_PARENTS = 3;

#endif // LIFE_H

