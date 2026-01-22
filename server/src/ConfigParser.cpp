#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &filename)
{
    std::ifstream configFileStream(filename);

    if (!configFileStream.is_open())
    {
        throw std::runtime_error("Failed to open " + filename);
    }
    try
    {
        _configData = json::parse(configFileStream);
    }
    catch (json::parse_error &e)
    {
        throw std::runtime_error("Parse error at byte " + e.byte);
    }
}

Config ConfigParser::parseConfig()
{
    try
    {
        Config config{
            .port = _configData.at("port"),
            .replaySpeed = _configData.at("replay_speed"),
            .nasdaqHistoricalFilePath = _configData.at("nasdaq_historical_file_path")};
        const auto &instruments = _configData.at("instruments");

        for (const auto &instrument : instruments)
        {
            const std::size_t &id = instrument.at("id");
            const std::string &symbol = instrument.at("symbol");
            const auto &specs = instrument.at("specifications");
            const Instrument::Specs parsedSpecs{.depth = specs.at("depth"),
                                                .enabled = specs.at("enabled")};

            config.instruments[id] = Instrument{.symbol = symbol, .specs = parsedSpecs};
        }
        return config;
    }
    catch (const json::out_of_range &e)
    {
        std::cerr << e.what() << std::endl;
        throw std::runtime_error("Failed to load required fields");
    }
}