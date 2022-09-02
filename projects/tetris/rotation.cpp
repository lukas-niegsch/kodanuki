#include "rotation.h"
#include "board.h"
#include "movement.h"
#include "tetromino.h"
#include "wallkick.h"
#include "engine/central/archetype.h"
using namespace Kodanuki;

TetrominoRotations calculate_tetromino_rotations()
{
	TetrominoRotations result;
	for (int t = 0; t < 7; t++) {
		Tetromino tetromino = create_tetromino(t);
		for (int r = 0; r < 4; r++) {
			Entity entity = ECS::create();
			ECS::update<Tetromino>(entity, tetromino);
			result.rotations[7 * r + t] = entity;
			rotate_tetromino(tetromino);
		}
	}
	return result;
}

template <typename Flag, int count>
void process_rotation_flags()
{
	using System = Archetype<Iterator<Entity, Rotation>, Predicate<Falling, Flag>>;
	for (auto[entity, rotation] : ECS::iterate<System>()) {
		rotation.target = (4 + rotation.source + count) % 4;
		ECS::remove<Flag>(entity);
	}
}

void rotate_tetromino_system()
{
	process_rotation_flags<RotateLeftFlag, -1>();
	process_rotation_flags<RotateRightFlag, 1>();
	using System = Archetype<Iterator<Entity, Tetromino, Position, Rotation, Board, TetrominoRotations>, Predicate<Falling>>;
	for (auto[entity, tetromino, position, rotation, board, base] : ECS::iterate<System>()) {
		int index = tetromino.type + 7 * rotation.target;
		Tetromino rotated = ECS::get<Tetromino>(base.rotations[index]);
		for (int attempt = 0; attempt < 5; attempt++) {
			auto offset = wallkick(rotation.source, rotation.target, tetromino.type, attempt);
			int localX = position.x + offset.first;
			int localY = position.y + offset.second;
			if (is_valid_position(board, rotated, localX, localY)) {
				position.x = localX;
				position.y = localY;
				ECS::bind<Tetromino>(entity, base.rotations[index]);
				rotation.source = rotation.target;
				break;
			}
		}
		if (rotation.source != rotation.target) {
			rotation.target = rotation.source;
		}
	}
}
