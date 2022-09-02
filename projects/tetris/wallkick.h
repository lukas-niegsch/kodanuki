#pragma once
#include <utility>

/**
 * Returns the (x, y) offset for a tetromino wallkick.
 * 
 * Tetris defines the Standard Rotation System where 5 attempts are made to
 * rotate a given tetromino. Each attempt defines an (x, y) pair on how to
 * move the tetromino after the rotation. If the tetromino fits inside the
 * board after some attempt, then the rotation is successful. If all 5 attempts
 * fail, then the tetromino does not rotate.
 * 
 * @param source The source rotation of the tetromino.
 * @param target The target rotation of the tetromino.
 * @param type The type of the tetromino.
 * @param attempt The current attempt of the wallkick.
 * @return The (x, y) ofset of the wallkick.
 */
std::pair<int, int> wallkick(int source, int target, int type, int attempt);
