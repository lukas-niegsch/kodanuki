#pragma once
#include "engine/nekolib/templates/stringify.h"
#include <iostream>
#include <stdexcept>


/**
 * Makro that terminates the program and prints some debug information.
 */
#define ERROR(reason)									\
	do {												\
		std::stringstream err;							\
		err << (reason) << '\n';						\
		err << "[Error] in file: " << __FILE__ << '\n';	\
		err << "[Error] in line: " << __LINE__ << '\n';	\
		throw std::runtime_error(err.str());    		\
	} while (false)


/**
 * Makro that checks if the function call was executed successfully.
 */
#define CHECK_RESULT(result, value)				\
	do {										\
		auto return_type = result;				\
		if (return_type != value) {				\
			ERROR(return_type);					\
		}										\
	} while (false)
