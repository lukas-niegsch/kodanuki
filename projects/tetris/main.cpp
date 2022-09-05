#include "board.h"
#include "graphics.h"
#include "movement.h"
#include "tetromino.h"
#include "rotation.h"
#include "engine/central/entity.h"
#include "engine/central/archetype.h"
#include <ncurses.h>
#include <array>
#include <vector>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace Kodanuki;

#define ON_EXIT 27
#define ON_ROTATE_LEFT 'q'
#define ON_ROTATE_RIGHT 'e'
#define ON_MOVE_DOWN 's'
#define ON_MOVE_LEFT 'a'
#define ON_MOVE_RIGHT 'd'
#define NUMBER_OF_BOARDS 3
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_SPACING 4

void print_score_line(int lines)
{
	std::stringstream output;
	output << "moving = [" << ON_MOVE_LEFT << " " << ON_MOVE_DOWN << " " << ON_MOVE_RIGHT << "]";
	output << "   ";
	output << "rotating = [" << ON_ROTATE_LEFT << " " << ON_ROTATE_RIGHT << "]";
	output << "   ";
	output << "exiting = [" << "Esc" << "]";
	output << "   ";
	output << "score = " << std::setw(6) << std::setfill('0') << lines;
	mvaddstr(1, 5, output.str().c_str());
}

/**
 * Creates a new falling tetromino.
 * 
 * @param world The entity containing the board and rotations.
 * @param speed The initial speed of the tetromino.
 * @return Entity The entity containing all relevant components.
 */
Entity create_falling_tetromino(Entity world, float speed)
{
	Entity tetromino = ECS::create();
	ECS::bind<Board>(tetromino, world);
	ECS::bind<TetrominoRotations>(tetromino, world);
	ECS::update<Rotation>(tetromino, {0, 0});
	ECS::update<Color>(tetromino, {1 + std::rand() % 6});
	ECS::update<Falling>(tetromino, {speed, 0});
	TetrominoRotations& rotations = ECS::get<TetrominoRotations>(tetromino);
	ECS::bind<Tetromino>(tetromino, rotations.rotations[std::rand() % 7]);
	int size = ECS::get<Tetromino>(tetromino).size;
	ECS::update<Position>(tetromino, {std::rand() % (BOARD_WIDTH - size), -2});
	return tetromino;
}

/**
 * Updates the given flag for all tetrominos.
 * 
 * @param tetrominos The given tetrominos.
 */
template <typename Flag>
void update_flag_globally(std::vector<std::vector<Entity>> tetrominos)
{
	for (auto subset : tetrominos) {
		for (Entity tetromino : subset) {
			ECS::update<Flag>(tetromino);
		}
	}
}

std::vector<Entity> create_boards()
{
	int width = BOARD_WIDTH;
	int height = BOARD_HEIGHT;
	int size = width * height;
	int spacing = BOARD_SPACING;
	std::vector<int> emptyBoard(size, 0);

	Entity mainBoard = ECS::create();
	ECS::update<Board>(mainBoard, {3, 4, width, height, emptyBoard, true});
	ECS::update<TetrominoRotations>(mainBoard, calculate_tetromino_rotations());

	std::vector<Entity> boards;
	boards.push_back(mainBoard);

	for (int i = 1; i < NUMBER_OF_BOARDS; i++) {
		Entity board = ECS::create();
		ECS::update<Board>(board, {3 + (width + spacing / 2) * i, 4, width, height, emptyBoard, true});
		ECS::bind<TetrominoRotations>(board, mainBoard);
		boards.push_back(board);
	}

	return boards;
}

void clean_boards(std::vector<Entity> boards)
{
	TetrominoRotations tetrominos = ECS::get<TetrominoRotations>(boards[0]);
	for (Entity entity : tetrominos.rotations) {
		ECS::remove<Entity>(entity);
	}
	for (Entity entity : boards) {
		ECS::remove<Entity>(entity);
	}
}

void clean_tetrominos(std::vector<std::vector<Entity>> tetrominos)
{
	for (auto entities : tetrominos) {
		for (Entity entity : entities) {
			ECS::remove<Entity>(entity);
		}
	}
}

int main()
{
	std::srand(std::time(0));
	NcursesRenderer::attach();
	std::vector<Entity> boards = create_boards();
	std::vector<std::vector<Entity>> tetrominos(boards.size());

	bool running = true;
	int lines = 0;
	float speed = 200;

	while (running)
	{
		speed += 0.01;

		for (int i = 0; i < (int) tetrominos.size(); i++) {
			tetrominos[i].erase(std::remove_if(tetrominos[i].begin(), tetrominos[i].end(),
				[](Entity entity){ return !ECS::has<Entity>(entity); }
			), tetrominos[i].end());
			if (tetrominos[i].empty()) {
				tetrominos[i].push_back(create_falling_tetromino(boards[i], speed));
			}
		}

		int input = getch();
		if (input == ON_EXIT) {
			running = false;
		} else if (input == ON_MOVE_LEFT) {
			update_flag_globally<MoveLeftFlag>(tetrominos);
		} else if (input == ON_MOVE_RIGHT) {
			update_flag_globally<MoveRightFlag>(tetrominos);
		} else if (input == ON_MOVE_DOWN) {
			update_flag_globally<MoveDownFlag>(tetrominos);
		} else if (input == ON_ROTATE_LEFT) {
			update_flag_globally<RotateLeftFlag>(tetrominos);
		} else if (input == ON_ROTATE_RIGHT) {
			update_flag_globally<RotateRightFlag>(tetrominos);
		}

		move_tetromino_system();
		rotate_tetromino_system();
		NcursesRenderer::render();

		for (Entity entity : boards) {
			Board& board = ECS::get<Board>(entity);
			lines += clear_lines(board);
			running &= board.playable;
		}

		print_score_line(lines);
	}

	NcursesRenderer::detach();
	clean_boards(boards);
	clean_tetrominos(tetrominos);
	std::cout << "You lost! Score: " << lines << '\n';
	return 0;
}
