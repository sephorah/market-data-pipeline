#ifndef CONFIGPARSER_HPP_
#define CONFIGPARSER_HPP_

#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct Instrument
{
    struct Specs
    {
        std::size_t depth;
        bool enabled;
    };
    std::string symbol;
    Specs specs;
};

struct Config
{
    std::size_t port;
    double replaySpeed;
    std::string nasdaqHistoricalFilePath;
    std::unordered_map<std::size_t, Instrument> instruments;
};

class ConfigParser
{
public:
    explicit ConfigParser(const std::string &filename);
    Config parseConfig();

private:
    json _configData;
};

#endif /* !CONFIGPARSER_HPP_ */