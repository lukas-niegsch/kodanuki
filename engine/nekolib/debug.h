#pragma once
#include "engine/nekolib/templates/stringify.h"
#include <iostream>
#include <stdexcept>


#define ERROR(reason)									\
	do {												\
		std::stringstream err;							\
		err << reason << '\n';							\
		err << "File: " << __FILE__ << '\n';			\
		err << "Line: " << __LINE__ << '\n';			\
		std::cout << err.str();							\
		std::terminate(); 								\
	} while (false)


#define CHECK_RESULT(result, value)						\
	do {												\
		auto return_type = result;						\
		if (return_type != value) {						\
			ERROR(stringify(return_type));			    \
		}												\
	} while (false)
