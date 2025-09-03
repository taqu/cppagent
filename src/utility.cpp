#include "utility.h"

namespace cppagent
{
std::string strip(std::string str)
{
    std::string::size_type start = str.find_first_not_of(" \t\n\r");
    std::string::size_type end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}
} // namespace cppagent
