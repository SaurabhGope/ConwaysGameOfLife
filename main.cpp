#include <iostream>
#include "grid.h"
#include "ansi_escapes.h"

int main(int argc, char* argv[])
{
	std::cout << "Conway's Game of Life \n";
	std::cout << "Press the return key to display each generation\n";

	// Wait for user
	std::cin.get();

	// Enable ANSI escape codes on Windows
	SetupConsole();

	// Grid for the first generation
	Grid currentGeneration;

	// Populate the cells at random
	currentGeneration.Randomize();

	while (true)
	{
		// Draw the current generation
		currentGeneration.Draw();

		// Wait for user to press the return key
		std::cin.get();

		// Grid for the next generation
		Grid nextGeneration;

		// Populate the cells in the next generation
		Calculate(currentGeneration, nextGeneration);

		// Update to the next generation
		currentGeneration.Update(nextGeneration);
	}

	// Move cursor to bottom of screen
	std::cout << "\x1b[" << 0 << ";" << ROW_MAX - 1 << "H";

	// Restore console on Windows
	RestoreConsole();
}
