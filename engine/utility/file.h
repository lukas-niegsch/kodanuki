#pragma once
#include <string>
#include <vector>

namespace Kodanuki
{

/**
 * Reads the complete file into the buffer.
 *
 * @param path The absolute or relative path of the file.
 * @return The buffer containing the contents of the file.
 */
std::vector<char> read_file_into_buffer(std::string path);

}

