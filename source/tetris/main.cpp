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

#define KEY_ESCAPE 27
#define KEY_ROTATE_LEFT 'q'
#define KEY_ROTATE_RIGHT 'e'
#define KEY_MOVE_LEFT 'a'
#define KEY_MOVE_DOWN 's'
#define KEY_MOVE_RIGHT 'd'
#define BOARD_COUNT 3
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_SPACING 4
#define INITIAL_SPEED 200

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

	for (int i = 1; i < BOARD_COUNT; i++) {
		Entity board = ECS::create();
		ECS::update<Board>(board, {3 + (width + spacing / 2) * i, 4, width, height, emptyBoard, true});
		ECS::bind<TetrominoRotations>(board, mainBoard);
		boards.push_back(board);
	}

	return boards;
}

void print_score_line(int score)
{
	std::stringstream output;
	output << "moving = [" << KEY_MOVE_LEFT << " " << KEY_MOVE_DOWN << " " << KEY_MOVE_RIGHT << "]";
	output << "   ";
	output << "rotating = [" << KEY_ROTATE_LEFT << " " << KEY_ROTATE_RIGHT << "]";
	output << "   ";
	output << "exiting = [" << "Esc" << "]";
	output << "   ";
	output << "score = " << std::setw(6) << std::setfill('0') << score;
	mvaddstr(1, 5, output.str().c_str());
}

void remove_entities(std::vector<Entity> entities)
{
	for (Entity entity : entities) {
		ECS::remove<Entity>(entity);
	}
}

template <typename Flag>
void update_entities(std::vector<Entity> entities)
{
	for (Entity entity : entities) {
		ECS::update<Flag>(entity);
	}
}

int main()
{
	std::srand(std::time(0));
	initialize_ncurses();
	std::vector<Entity> boards = create_boards();
	std::vector<Entity> tetrominos(boards.size());

	float speed = INITIAL_SPEED;
	int lines = 0;
	bool running = true;

	while (running)
	{
		switch (getch())
		{
		case KEY_ESCAPE:
			running = false;
			break;
		case KEY_MOVE_LEFT:
			update_entities<MoveLeftFlag>(tetrominos);
			break;
		case KEY_MOVE_RIGHT:
			update_entities<MoveRightFlag>(tetrominos);
			break;
		case KEY_MOVE_DOWN:
			update_entities<MoveDownFlag>(tetrominos);
			break;
		case KEY_ROTATE_LEFT:
			update_entities<RotateLeftFlag>(tetrominos);
			break;
		case KEY_ROTATE_RIGHT:
			update_entities<RotateRightFlag>(tetrominos);
			break;
		}

		for (Entity entity : boards) {
			Board& board = ECS::get<Board>(entity);
			lines += clear_lines(board);
			running &= board.playable;
		}

		for (int i = 0; i < (int) boards.size(); i++) {
			if (tetrominos[i] && ECS::has<Entity>(tetrominos[i])) {
				continue;
			}
			tetrominos[i] = create_falling_tetromino(boards[i], speed);
		}

		speed += 0.01;
		move_tetromino_system();
		rotate_tetromino_system();
		draw_board_system();
		draw_tetromino_system();
		print_score_line(lines);
	}

	auto rotations = ECS::get<TetrominoRotations>(boards[0]).rotations;
	remove_entities(std::vector<Entity>(rotations.begin(), rotations.end()));
	remove_entities(boards);
	remove_entities(tetrominos);
	terminate_ncurses();
	std::cout << "You lost! Score: " << lines << '\n';
	return 0;
}
