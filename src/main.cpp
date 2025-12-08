#include <iostream>
#include <driver/ArgumentParser.hpp>
#include <driver/CompilerDriver.hpp>

int main(int argc, char** argv)
{
    try {
        ArgumentParser parser(argc, argv);
        CompilerOptions opts = parser.parse();
        
        if (opts.showHelp) {
            ArgumentParser::printUsage(argv[0]);
            return 0;
        }
        
        if (opts.showVersion) {
            ArgumentParser::printVersion();
            return 0;
        }
        
        CompilerDriver driver(opts);
        return driver.compile() ? 0 : 1;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}