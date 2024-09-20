// configreader.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct Config_params {
    double LineRate = 0;
    int CaptureSizeMs = 0;
    int MinNumOfIFGsPerPacket = 0;
    int BurstSize = 0;
    long long BurstPeriodicity_us = 0;
    std::string DestAddress;
    std::string SourceAddress;
    int MaxPacketSize = 0;
};

// Function to load configuration from a file
Config_params loadConfig(const std::string& filePath) {
    Config_params config;
    std::ifstream configFile(filePath);

    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream ss(line);
        std::string key, value;

        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            if (key == "Eth.LineRate") config.LineRate = std::stod(value);
            else if (key == "Eth.CaptureSizeMs") config.CaptureSizeMs = std::stoi(value);
            else if (key == "Eth.MinNumOfIFGsPerPacket") config.MinNumOfIFGsPerPacket = std::stoi(value);
            else if (key == "Eth.BurstSize") config.BurstSize = std::stoi(value);
            else if (key == "Eth.BurstPeriodicity_us") config.BurstPeriodicity_us = std::stoll(value);
            else if (key == "Eth.DestAddress") config.DestAddress = value;
            else if (key == "Eth.SourceAddress") config.SourceAddress = value;
            else if (key == "Eth.MaxPacketSize") config.MaxPacketSize = std::stoi(value);
        }
    }

    configFile.close();
    return config;
}

// Print Configuration
void printConfig(const Config_params& config, std::ostream& out) {
    out << "LineRate: " << config.LineRate << std::endl;
    out << "CaptureSizeMs: " << config.CaptureSizeMs << std::endl;
    out << "MinNumOfIFGsPerPacket: " << config.MinNumOfIFGsPerPacket << std::endl;
    out << "BurstSize: " << config.BurstSize << std::endl;
    out << "BurstPeriodicity_us: " << config.BurstPeriodicity_us << std::endl;
    out << "DestAddress: " << config.DestAddress << std::endl;
    out << "SourceAddress: " << config.SourceAddress << std::endl;
    out << "MaxPacketSize: " << config.MaxPacketSize << std::endl;
}
