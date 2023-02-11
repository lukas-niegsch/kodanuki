#pragma once
#include <functional>
#include <vector>

/**
 * Each tetromino consists of 4 blocks that are orthogonally connected.
 * 
 * The blocks are embedded inside a bounding box of size x size. They are
 * stored inside a single vector and checked with isblock[size * y + x].
 * 
 * Tetrominos should only be created using the method below.
 */
struct Tetromino
{
	// The type of the tetromino modulo 7.
	int type;
	
	// The length of one side of the bounding box.
	int size;

	// The array to check wether a block is at the position.
	std::vector<bool> isblock;
};

/**
 * Creates one of the 7 types of tetrominos.
 * 
 * @param type The type of the tetromino.
 * @return Tetromino The created tetromino.
 */
Tetromino create_tetromino(int type);

/**
 * Executes the given callback for all blocks.
 * 
 * @param tetromino The tetromino for which blocks are considered.
 * @param callback The callback executing with (x, y) positions of blocks.
 */
void execute_blockwise(Tetromino tetromino, std::function<void(int, int)> callback);

/**
 * Rotates the given tetromino inplace clockwise.
 * 
 * @param tetromino The tetromino to rotate.
 */
void rotate_tetromino(Tetromino& tetromino);
