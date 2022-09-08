#include "board.h"
#include <algorithm>

int& Board::operator() (int x, int y)
{
	return *(isblock.begin() + y * sizeX + x); 
}

bool is_block_inside_board(Board board, int x, int y)
{
	return x >= 0 && y < board.sizeY && x < board.sizeX;
}

bool is_valid_position(Board board, Tetromino tetromino, int x, int y)
{
	bool invalid = false;
	execute_blockwise(tetromino, [&](int blockX, int blockY){
		int posX = x + blockX;
		int posY = y + blockY;
		invalid |= !is_block_inside_board(board, posX, posY);
		int index = std::clamp(posY * board.sizeX + posX, 0, (int) board.isblock.size() - 1);
		invalid |= board.isblock[index];
	});
	return !invalid;
}

void fixate_tetromino(Board& board, Tetromino tetromino, int color, int x, int y)
{
	execute_blockwise(tetromino, [&](int blockX, int blockY) {
		int boardX = x + blockX;
		int boardY = y + blockY;
		int index = boardY * board.sizeX + boardX;
		if (index < 0 || index >= board.sizeX * board.sizeY) {
			board.playable = false;
		} else {
			board.isblock[index] = color;
		}
	});
}

int clear_lines(Board& board)
{
	int k = board.sizeY - 1;
	for (int y = k; y > 0; y--) {
		int count = 0;
		for (int x = 0; x < board.sizeX; x++) {
			count += board(x, y) > 0;
			board(x, k) = board(x, y);
		}
		k -= count < board.sizeX;
	}
	return k;
}
