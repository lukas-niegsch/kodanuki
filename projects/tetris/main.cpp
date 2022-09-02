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
	int boardWidth = ECS::get<Board>(world).sizeX;
	ECS::update<Position>(tetromino, {3 + std::rand() % (boardWidth - 2 * 3), -2});
	ECS::update<Falling>(tetromino, {speed, 0});
	TetrominoRotations& rotations = ECS::get<TetrominoRotations>(tetromino);
	ECS::bind<Tetromino>(tetromino, rotations.rotations[std::rand() % 7]);
	return tetromino;
}

/**
 * Destroys a falling tetromino.
 * 
 * @param tetromino The entity containing all relevant components.
 */
void destroy_falling_tetromino(Entity tetromino)
{
	ECS::remove<Board>(tetromino);
	ECS::remove<TetrominoRotations>(tetromino);
	ECS::remove<Rotation>(tetromino);
	ECS::remove<Color>(tetromino);
	ECS::remove<Position>(tetromino);
	ECS::remove<Falling>(tetromino);
	ECS::remove<Tetromino>(tetromino);
}

/**
 * Destroys the entity if it isn't falling.
 * 
 * @param entity The entity that is potentially destroyed.
 * @return Was the entity destroyed?
 */
bool destroy_not_falling(Entity entity) {
	if (!ECS::has<Falling>(entity)) {
		destroy_falling_tetromino(entity);
		return true;
	}
	return false;
}

/**
 * Updates the given flag for all tetrominos.
 * 
 * @param tetrominos The given tetrominos.
 */
template <typename Flag>
void update_flag_globally(std::array<std::vector<Entity>, 3> tetrominos)
{
	for (auto subset : tetrominos) {
		for (Entity tetromino : subset) {
			ECS::update<Flag>(tetromino);
		}
	}
}

int main()
{
	std::srand(std::time(0));
	NcursesRenderer::attach();

	int width = 10;
	int height = 20;
	int size = width * height;
	std::vector<int> emptyBoard(size, 0);


	Entity boardLeft = ECS::create();
	ECS::update<Board>(boardLeft, {3, 4, width, height, emptyBoard, true});
	ECS::update<TetrominoRotations>(boardLeft, calculate_tetromino_rotations());

	Entity boardMiddle = ECS::create();
	ECS::update<Board>(boardMiddle, {15, 4, width, height, emptyBoard, true});
	ECS::bind<TetrominoRotations>(boardMiddle, boardLeft);

	Entity boardRight = ECS::create();
	ECS::update<Board>(boardRight, {27, 4, width, height, emptyBoard, true});
	ECS::bind<TetrominoRotations>(boardRight, boardLeft);

	std::array<Entity, 3> boards = {boardLeft, boardMiddle, boardRight};
	std::array<std::vector<Entity>, 3> tetrominos;
	
	bool running = true;
	int lines = 0;
	float speed = 200;

	while (running)
	{
		speed += 0.1;
		
		for (int i = 0; i < (int) tetrominos.size(); i++) {
			for (auto it = tetrominos[i].begin(); it != tetrominos[i].end();) {
				if (destroy_not_falling(*it)) {
					it = tetrominos[i].erase(it);
				} else {
					it++;
				}
			}
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
	std::cout << "You lost! Score: " << lines << '\n';
	return 0;
}
