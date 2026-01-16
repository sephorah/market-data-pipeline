#include "Server.hpp"
#include "ArgParser.hpp"

int main(const int ac, const char *const *av)
{
    try
    {
        ArgParser parser(ac, av);

        if (parser.checkHelp())
        {
            return EXIT_SUCCESS;
        }
        parser.parseArgs();
        std::cout << "Filename : " << parser.filename << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}