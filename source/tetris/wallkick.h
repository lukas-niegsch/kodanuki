#pragma once
#include <utility>

/**
 * Returns the (x, y) offset for a tetromino wallkick.
 * 
 * We use the Standard Rotation System where 5 attempts are made to rotate
 * a given tetromino. Each attempt defines (x, y) offsets on how to move a
 * tetromino after the rotation. The rotation attempt succeeds if there is
 * space inside. After 5 failing attempts, the tetromino does not rotate.
 * We modified the "I" rotation slightly in attempt 2 for symmetry.
 * 
 * @param source The source rotation of the tetromino.
 * @param target The target rotation of the tetromino.
 * @param type The type of the tetromino.
 * @param attempt The attempt of the wallkick.
 * @return The (x, y) offset of the wallkick.
 */
std::pair<int, int> wallkick(int source, int target, int type, int attempt);
