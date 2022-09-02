#include "board.h"
#include <algorithm>

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
	int count = 0;
	for (int y = 0; y < board.sizeY; y++) {
		bool isFilled = true;
		for (int x = 0; x < board.sizeX; x++) {
			if (!board.isblock[y * board.sizeX + x]) {
				isFilled = false;
				break;
			}
		}
		if (isFilled) {
			count++;
			for (int y1 = y; y1 > 0; y1--) {
				std::swap_ranges(board.isblock.begin() + board.sizeX * y1,
					board.isblock.begin() + board.sizeX * y1 + board.sizeX,
					board.isblock.begin() + board.sizeX * (y1 - 1));
			}
			for (int x = 0; x < board.sizeX; x++) {
				board.isblock[x] = 0;
			}
		}
	}
	return count;
}
