#pragma once
#include "engine/central/entity.h"
#include <array>

/**
 * Describes one tetromino rotation.
 * 
 * This component should be added to an tetromino. Afterward this rotation
 * will be applied at the next call of the rotation system. Integers are given
 * mod4 with 0 being the null rotation and r + k representing k 90% rotations
 * clockwise from the r rotation.
 */
struct Rotation
{
	// The source rotation.
	int source;

	// The target rotation.
	int target;
};

/**
 * The possible tetromino rotations.
 * 
 * Each Entity has a tetromino component representing one rotation for a
 * given type. The specific tetromino can be bind using "type + 7 * rotation."
 */
struct TetrominoRotations
{
	// The array containing combinations of tetromino type and rotations.
	std::array<Kodanuki::Entity, 28> rotations;
};

// Consumable component flag indicating one counter-clockwise rotation.
struct RotateLeftFlag {};

// Consumable component flag indicating one clockwise rotation.
struct RotateRightFlag {};

/**
 * Calculates all possible tetromino rotations.
 * 
 * @return The possible tetromino rotations.
 */
TetrominoRotations calculate_tetromino_rotations();

/**
 * Rotates all tetrominos once.
 * 
 * Only tetrominos with the rotation component are rotated. It will consume
 * the component. Entities must also have the other components: Tetromino,
 * Position, Rotation, Board, TetrominoRotations.
 */
void rotate_tetromino_system();
