#include "ArgParser.hpp"

ArgParser::ArgParser(const int ac, const char *const *av) : _options(cxxopts::Options("MarketDataSimulatorServer", "A C++ server disseminating market data."))
{
    const std::string helpMessage = "Print help message";

    _options.add_options()("f, filename", "File name", cxxopts::value<std::string>())("h,help", helpMessage);
    _parsedArgs = _options.parse(ac, av);
}

bool ArgParser::checkHelp()
{
    if (_parsedArgs.count("help"))
    {
        std::cout << _options.help() << std::endl;
        return true;
    }
    return false;
}

ParsedArgs ArgParser::parseArgs()
{
    ParsedArgs args;

    if (_parsedArgs.count("filename"))
    {
        args.filename = _parsedArgs["filename"].as<std::string>();
    }
    else
    {
        throw std::runtime_error("Filename is required.");
    }
    return args;
}
