#pragma once
#include "tetromino.h"
#include <vector>

/**
 * Each board contains several blocks represented as a boolean value.
 * 
 * The board also translate board coordinates used by the rest of the
 * application to global coordinates using by the graphics. To calculate
 * the global values, just add the offset to (x, y).
 */
struct Board
{
	// The global offset in x direction.
	int offsetX;

	// The global offset in y direction.
	int offsetY;

	// The width of the board.
	int sizeX;

	// The height of the board.
	int sizeY;

	// The array to check wether a block is at the position.
	std::vector<int> isblock;

	// Is the board already lost?
	bool playable;

	// The element at position y * sizeX + x.
	int& operator() (int x, int y);
};

/**
 * Checks wether the board contains a block.
 * 
 * @param board The board that contains blocks.
 * @param x The x position of the block to check.
 * @param y The y position of the block to check.
 * @return Is the block inside the board?
 */
bool is_block_inside_board(Board board, int x, int y);

/**
 * Checks wether the tetromino fits inside the board.
 * 
 * @param board The board that contains blocks.
 * @param tetromino The tetromino that should be checked.
 * @param x The x position of the tetromino.
 * @param y The y position of the tetromino.
 * @return Does the tetromino fit at the given position? 
 */
bool is_valid_position(Board board, Tetromino tetromino, int x, int y);

/**
 * Fixates the teromino onto the board.
 * 
 * @param board The board that contains blocks.
 * @param tetromino The tetromino that should be fixated.
 * @param x The x position of the tetromino.
 * @param y The y position of the tetromino.
 */
void fixate_tetromino(Board& board, Tetromino tetromino, int color, int x, int y);

/**
 * Checks if any lines are complete and clears them.
 * 
 * @return The number of lines cleared.
 */
int clear_lines(Board& board);
