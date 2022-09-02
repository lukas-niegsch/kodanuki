#pragma once
#include "engine/central/entity.h"

/**
 * The position of a tetromino inside the board.
 */
struct Position
{
	// The left position of the tetromino bounding box. 
	int x;

	// The top position of the tetromino bounding box. 
	int y;
};

/**
 * Structure that describes the falling of tetrominos.
 */
struct Falling
{
	// The speed in which the tetromino should fall. (higher = faster)
	float speed;

	// The countdown for the next down movement.
	int countdown;
};

// Consumable component flag indicating one left movement.
struct MoveLeftFlag {};

// Consumable component flag indicating one right movement.
struct MoveRightFlag {};

// Consumable component flag indicating one down movement.
struct MoveDownFlag {};

/**
 * The movement system takes all the movement flags and updates
 * the state of the tetromino accordingly. It will consume the movement
 * flags of all components.
 */
void move_tetromino_system();
