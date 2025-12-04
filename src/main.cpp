#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <driver/CompilerDriver.hpp>
#include <utils/color.hpp>

int main(int ac, char** av)
{
    try
    {
        if (ac == 1)
            throw std::invalid_argument("no input file");
        
        std::vector<std::string> input_files;
        std::string output_file;
        
        for (int i = 1; i < ac; ++i)
        {
            std::string arg = av[i];
            if (arg == "-o" && i + 1 < ac)
                output_file = av[++i];
            else if (arg.substr(0, 2) == "-o")
                output_file = arg.substr(2);
            else
                input_files.emplace_back(arg);
        }
        
        if (input_files.empty())
            throw std::invalid_argument("no input file");

        CompilerDriver driver(input_files, output_file);
        if (!driver.compile())
            return -1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}