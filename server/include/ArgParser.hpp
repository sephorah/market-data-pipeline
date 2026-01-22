#ifndef ARGPARSER_HPP_
#define ARGPARSER_HPP_

#include <string>
#include <iostream>
#include "cxxopts.hpp"

#define MAX_ARGS 2

struct ParsedArgs
{
    std::string filename;
};

class ArgParser
{
public:
    explicit ArgParser(const int ac, const char *const *av);
    ParsedArgs parseArgs();
    bool checkHelp();

private:
    cxxopts::Options _options;
    cxxopts::ParseResult _parsedArgs;
};

#endif /* !ARGPARSER_HPP_ */