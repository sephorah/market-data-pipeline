#include "Server.hpp"
#include "ArgParser.hpp"
#include "ConfigParser.hpp"

int main(const int ac, const char *const *av)
{
    try
    {
        ArgParser argsParser(ac, av);

        if (argsParser.checkHelp())
        {
            return EXIT_SUCCESS;
        }
        ParsedArgs args = argsParser.parseArgs();
        std::cout << "Filename : " << args.filename << std::endl;
        ConfigParser configParser(args.filename);
        Config config = configParser.parseConfig();
        std::cout << "CONFIG" << std::endl;
        std::cout << config.port << std::endl
                  << config.replaySpeed << std::endl
                  << config.nasdaqHistoricalFilePath << std::endl;
        for (const auto &[key, value] : config.instruments)
        {
            std::cout << key << " " << value.symbol << " " << value.specs.depth << " " << value.specs.enabled << " " << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}