#include <iostream>
#include <driver/ArgumentParser.hpp>
#include <driver/CompilerDriver.hpp>

// EN: Entry point; parses CLI, dispatches compile pipeline, handles errors.
// FR: Point d entree; parse CLI, lance le pipeline, gere les erreurs.
int main(int argc, char** argv)
{
    try {
        // EN: Parse command-line options into compiler settings.
        // FR: Parse la ligne de commande en options de compilation.
        ArgumentParser parser(argc, argv);
        CompilerOptions opts = parser.parse();
        
        // EN: Handle help/version requests without running the compiler.
        // FR: Gere help/version sans lancer le compilateur.
        if (opts.showHelp) {
            ArgumentParser::printUsage(argv[0]);
            return 0;
        }
        
        if (opts.showVersion) {
            ArgumentParser::printVersion();
            return 0;
        }
        
        // EN: Run the compiler driver; return status as exit code.
        // FR: Execute le driver; renvoie le status en code de sortie.
        CompilerDriver driver(opts);
        return driver.compile() ? 0 : 1;
    }
    catch (const std::exception& e) {
        // EN: Report fatal errors and signal failure.
        // FR: Signale les erreurs fatales et renvoie un echec.
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
