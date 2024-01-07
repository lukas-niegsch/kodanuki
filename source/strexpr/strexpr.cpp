#include "source/strexpr/strexpr.hpp"
#include <iostream>


int main()
{
	std::vector<int> numbers = {3, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2,
	                            3, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2,
	                            3, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2};
	strexpr::range_query naive_rmq  = strexpr::naive_range_minimum_query(numbers);
	strexpr::range_query oneoff_rmq = strexpr::oneoff_range_minimum_query(numbers);


	for (std::size_t i = 0; i < numbers.size(); i++) {
		for (std::size_t j = i; j < numbers.size(); j++) {
			int expect = std::numeric_limits<int>::max();
			for (std::size_t k = i; k <= j; k++) {
				expect = std::min(expect, numbers[k]);
			}
			int naive_actual = numbers[naive_rmq(i, j)];
			int oneoff_actual = numbers[oneoff_rmq(i, j)];
			
			if (expect != oneoff_actual || expect != naive_actual) {
				std::cout << std::endl;
				std::cout << "rmq(" << i << ", " << j << "):" << std::endl;
				std::cout << "expect:        " << expect << std::endl;
				std::cout << "naive_actual:  " << naive_actual << std::endl;
				std::cout << "oneoff_actual: " << oneoff_actual << std::endl;
			}
		}
	}
	return EXIT_SUCCESS;
}
