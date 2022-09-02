#include "wallkick.h"

// TODO: there are some errors inside these values for the I piece
#define WALLKICK_OFFSET_0N {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}
#define WALLKICK_OFFSET_1N {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}
#define WALLKICK_OFFSET_2N WALLKICK_OFFSET_0N
#define WALLKICK_OFFSET_3N {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
#define WALLKICK_OFFSET_0I {{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}}
#define WALLKICK_OFFSET_1I {{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}}
#define WALLKICK_OFFSET_2I {{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}}
#define WALLKICK_OFFSET_3I {{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}}

template <typename T,typename U> 
std::pair<T, U> operator-(const std::pair<T, U>& a, const std::pair<T, U>& b)
{
	return {a.first - b.first, a.second - b.second};                                    
}

std::pair<int, int> wallkick(int source, int target, int type, int attempt)
{
	if (type == 0) {
		std::pair<int, int> offsets[4][5] =
			{WALLKICK_OFFSET_0I, WALLKICK_OFFSET_1I, WALLKICK_OFFSET_2I, WALLKICK_OFFSET_3I};
		return offsets[source][attempt] - offsets[target][attempt];
	} else if (type == 3) {
		return {0, 0}; // rotation O piece does nothing
	} else {
		std::pair<int, int> offsets[4][5] =
			{WALLKICK_OFFSET_0N, WALLKICK_OFFSET_1N, WALLKICK_OFFSET_2N, WALLKICK_OFFSET_3N};
		return offsets[source][attempt] - offsets[target][attempt];
	}
}
