#include "engine/nekolib/parsing/file.h"
#include <stdexcept>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

namespace kodanuki
{

std::vector<char> read_file_into_buffer(std::string path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		throw std::invalid_argument(path + " was not found!");
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size))
	{
		throw std::runtime_error(path + " could not be read!");
	}

	return buffer;
}

}

