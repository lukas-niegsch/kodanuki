#pragma once

/**
 * The color for some tetromino.
 */
struct Color
{
	// The ncurses color value.
	int ncurses_mod8;
};

/**
 * The renderer for the tetris game that uses the ncurses library.
 * 
 * The renderer uses the ECS to iterate over all the entities that
 * should be rendered: Tetromino, Color, Position, Board.
 */
namespace NcursesRenderer
{
	// Attach this renderer and initialize it.
	void attach();

	// Detach this renderer and terminate it.
	void detach();

	// Render all tetrominos and all boards.
	void render();
};
