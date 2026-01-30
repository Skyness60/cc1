#include "utils/color.hpp"

namespace utils
{
    // EN: Wraps text with an ANSI color code and a reset code.
    // FR: Encadre le texte avec un code couleur ANSI et un reset.
    std::string color_text(const std::string& text, const std::string& color_code)
    {
        return color_code + text + "\033[0m"; 
    }
}
