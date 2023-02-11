#include "movement.h"
#include "board.h"
#include "graphics.h"
#include "tetromino.h"
#include "engine/central/entity.h"
#include "engine/central/archetype.h"
using namespace kodanuki;

template <typename Flag, int count>
void move_horizontal_system()
{
	using System = Archetype<Iterate<Entity, Tetromino, Position, Board>, Require<Falling>, Consume<Flag>>;
	for (auto[entity, tetromino, position, board] : ECS::iterate<System>()) {
		if (is_valid_position(board, tetromino, position.x + count, position.y)) {
			position.x += count;
		}
	}
}

template <typename Flag, int count>
void move_vertical_system()
{
	using System = Archetype<Iterate<Entity, Tetromino, Color, Position, Board>, Require<Falling>, Consume<Flag>>;
	for (auto[entity, tetromino, color, position, board] : ECS::iterate<System>()) {
		if (is_valid_position(board, tetromino, position.x, position.y  + count)) {
			position.y += count;
		} else {
			fixate_tetromino(board, tetromino, color.ncurses_mod8, position.x, position.y);
			ECS::remove<Entity>(entity);
		}
	}
}

void countdown_system()
{
	using System = Archetype<Iterate<Entity, Falling>>;
	for (auto[entity, falling] : ECS::iterate<System>()) {
		falling.countdown--;
		if (falling.countdown < 0) {
			falling.countdown = 10000 / falling.speed;
			ECS::update<MoveDownFlag>(entity);
		}
	}
}

void move_tetromino_system()
{
	move_horizontal_system<MoveLeftFlag, -1>();
	move_horizontal_system<MoveRightFlag, 1>();
	move_vertical_system<MoveDownFlag, 1>();
	countdown_system();
}
