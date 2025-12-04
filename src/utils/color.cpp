#include "utils/color.hpp"

namespace utils
{
    std::string color_text(const std::string& text, const std::string& color_code)
    {
        return color_code + text + "\033[0m"; // Reset color
    }
}