#pragma once
#include <doctest/doctest.h>
#include <vector>

template <typename T, typename U>
void CHECK_VECTOR_EQUALS(std::vector<T> a, std::vector<U> b)
{
	if (a.size() != b.size()) {
		CHECK_MESSAGE(a.size() == b.size(), "Vectors are not the same size!");
		return;
	}

	for (std::size_t i = 0; i < a.size(); i++) {
		T va = a[i];
		U vb = b[i];
		std::stringstream ss;
		ss << "Vectors are different at index = ";
		ss << i;
		CHECK_MESSAGE(va == vb, ss.str());
	}
}
