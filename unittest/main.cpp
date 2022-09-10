#include "engine/central/entity.h"
#include "engine/central/archetype.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <array>
using namespace Kodanuki;

template <typename T, std::size_t width, std::size_t height>
class Matrix
{
public:
	const auto& operator[] (std::size_t x, std::size_t y) const
	{
		return data[y * width + x];
	}
	
private:
	std::array<T, width * height> data;
};

TEST_CASE("playground")
{
	const Matrix<int, 5, 5> matrix {};
	CHECK(matrix[1, 1] == 0);
}
