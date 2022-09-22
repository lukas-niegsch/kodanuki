#include "tetromino.h"

#define TETROMINO_I {0, 4, {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}}
#define TETROMINO_J {1, 3, {1, 0, 0, 1, 1, 1, 0, 0, 0}}
#define TETROMINO_L {2, 3, {0, 0, 1, 1, 1, 1, 0, 0, 0}}
#define TETROMINO_O {3, 2, {1, 1, 1, 1}}
#define TETROMINO_S {4, 3, {0, 1, 1, 1, 1, 0, 0, 0, 0}}
#define TETROMINO_T {5, 3, {0, 1, 0, 1, 1, 1, 0, 0, 0}}
#define TETROMINO_Z {6, 3, {1, 1, 0, 0, 1, 1, 0, 0, 0}}

Tetromino create_tetromino(int type)
{
	switch (type % 7)
	{
	case 0: return TETROMINO_I;
	case 1: return TETROMINO_J;
	case 2: return TETROMINO_L;
	case 3: return TETROMINO_O;
	case 4: return TETROMINO_S;
	case 5: return TETROMINO_T;
	case 6: return TETROMINO_Z;
	default: return {};
	}
};

void execute_blockwise(Tetromino tetromino, std::function<void(int, int)> callback)
{
	for (int x = 0; x < tetromino.size; x++) {
		for (int y = 0; y < tetromino.size; y++) {
			if (tetromino.isblock[tetromino.size * y + x]) {
				callback(x, y);
			}
		}
	}
}

void rotate_tetromino(Tetromino& tetromino)
{
	Tetromino copy = tetromino;
	std::fill(tetromino.isblock.begin(), tetromino.isblock.end(), false);
	int n = tetromino.size;
	execute_blockwise(copy, [&](int x, int y) {
		int indexOld = n * y + x;
		int indexNew = n * x + n - y - 1;
		tetromino.isblock[indexNew] = copy.isblock[indexOld];
	});
}
