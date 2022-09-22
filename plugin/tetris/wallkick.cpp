#include "wallkick.h"

#define WALLKICK_0N {{0, 0}, {1, 0}, {1, 1}, {0, 0}, {1, 0}}
#define WALLKICK_1N {{0, 0}, {2, 0}, {2, 0}, {0, 2}, {2, 2}}
#define WALLKICK_2N {{0, 0}, {1, 0}, {1, 1}, {0, 0}, {1, 0}}
#define WALLKICK_3N {{0, 0}, {0, 0}, {0, 0}, {0, 2}, {0, 2}}
#define WALLKICK_0I {{0, 0}, {0, 0}, {3, 0}, {0, 2}, {3, 2}}
#define WALLKICK_1I {{0, 0}, {2, 0}, {2, 0}, {2, 3}, {2, 0}}
#define WALLKICK_2I {{0, 0}, {2, 0}, {0, 0}, {5, 1}, {0, 1}}
#define WALLKICK_3I {{0, 0}, {0, 0}, {1, 0}, {3, 0}, {1, 3}}
#define WALLKICK_0O {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}
#define WALLKICK_1O {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}
#define WALLKICK_2O {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}
#define WALLKICK_3O {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}

static std::pair<int, int> wallkick_table[3][4][5] =
{
	{WALLKICK_0I, WALLKICK_1I, WALLKICK_2I, WALLKICK_3I},
	{WALLKICK_0O, WALLKICK_1O, WALLKICK_2O, WALLKICK_3O},
	{WALLKICK_0N, WALLKICK_1N, WALLKICK_2N, WALLKICK_3N},
};

std::pair<int, int> wallkick(int source, int target, int type, int attempt)
{
	int index = (type == 0 ? 0 : 1) * (type == 3 ? 1 : 2);
	auto offsetA = wallkick_table[index][source][attempt];
	auto offsetB = wallkick_table[index][target][attempt];
	return {offsetA.first - offsetB.first, offsetA.second - offsetB.second};
}
