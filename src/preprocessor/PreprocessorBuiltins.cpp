#include <preprocessor/Preprocessor.hpp>

#include <ctime>

namespace cc1 {

// EN: Defines builtin macros like __DATE__/__TIME__ and platform markers.
// FR: Definit les macros internes comme __DATE__/__TIME__ et marqueurs plateforme.
void Preprocessor::setupBuiltinMacros() {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    char dateBuf[32];
    std::strftime(dateBuf, sizeof(dateBuf), "\"%b %d %Y\"", tm);
    macroTable_.define("__DATE__", pp::MacroDefinition("__DATE__", std::vector<std::string>(), dateBuf, false, false));

    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "\"%H:%M:%S\"", tm);
    macroTable_.define("__TIME__", pp::MacroDefinition("__TIME__", std::vector<std::string>(), timeBuf, false, false));

    macroTable_.define("__STDC__", pp::MacroDefinition("__STDC__", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__STDC_VERSION__", pp::MacroDefinition("__STDC_VERSION__", std::vector<std::string>(), "199409L", false, false));

    macroTable_.define("__i386__", pp::MacroDefinition("__i386__", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__i386", pp::MacroDefinition("__i386", std::vector<std::string>(), "1", false, false));
    macroTable_.define("i386", pp::MacroDefinition("i386", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__linux__", pp::MacroDefinition("__linux__", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__unix__", pp::MacroDefinition("__unix__", std::vector<std::string>(), "1", false, false));
}

// TODO(cc1) EN: Make builtin macros configurable by target instead of hardcoding.
// FR: Rendre les macros internes configurables par cible au lieu de hardcoder.
} 
