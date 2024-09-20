// test.cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include "packetgenerator.cpp" // Include the implementation of packet generator
#include "configreader.cpp" // Include the implementation of config reader
#include "BurstGenerator.cpp" // Include the implementation of burst generator

std::string hexToPlainString(const std::string& hexAddress) {
    // Check if the address has the "0x" prefix and remove it
    std::string address = hexAddress.substr(0, 2) == "0x" ? hexAddress.substr(2) : hexAddress;

    // Ensure the length is correct (12 characters for a MAC address)
    if (address.length() != 12) {
        throw std::invalid_argument("Invalid MAC address length.");
    }

    return address; // Return the plain address without '0x' prefix
}

// Function to write bytes to a file in hexadecimal format, ensuring 4-byte alignment
void writeToFile(const std::string& filePath, const std::vector<uint8_t>& data) {
    std::ofstream outputFile(filePath);

    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << filePath << std::endl;
        return;
    }

    size_t padding = (4 - (data.size() % 4)) % 4;
    std::vector<uint8_t> paddedData = data;
    paddedData.insert(paddedData.end(), padding, 0x00); // Add padding bytes

    // Write data to the file in hexadecimal format, 4 bytes per line
    for (size_t i = 0; i < paddedData.size(); ++i) {
        outputFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(paddedData[i]);
        if ((i + 1) % 4 == 0) { // Print 4 bytes per line
            outputFile << std::endl;
        } else {
            outputFile << ' ';
        }
    }

    // Ensure the file ends with a newline if not empty
    if (!paddedData.empty()) {
        outputFile << std::endl;
    }

    outputFile.close();
    std::cout << "Data written to " << filePath << std::endl;
}

int main() {
    // File paths
    std::string configFilePath = "input.txt";  // Path to the configuration file
    std::string outputFilePath = "output.txt"; // Path to the output file
    // Open the input file
    std::ifstream inputFile(configFilePath);
    // Open the output file
    std::ofstream outputFile(outputFilePath);
    // Load configuration
    Config_params Config_params = loadConfig(configFilePath);

    // Print the configuration to the terminal
    std::cout << "Configuration read from " << configFilePath << ":" << std::endl;
    printConfig(Config_params, std::cout);
    // Dump the configuration to the output file
    outputFile << "Configuration read from " << configFilePath << ":" << std::endl;
    printConfig(Config_params, outputFile);

    // Close the files
    inputFile.close();
    outputFile.close();

    std::cout << "Configuration written to " << outputFilePath << std::endl;

    // Create a sample configuration
    Config_pkt config;
  //  config.LineRate = Config_params.LineRate;
  //  config.CaptureSizeMs = Config_params.CaptureSizeMs;
  //  config.MinNumOfIFGsPerPacket = Config_params.MinNumOfIFGsPerPacket;
  //  config.BurstSize = Config_params.BurstSize;
  //  config.BurstPeriodicity_us = Config_params.BurstPeriodicity_us;
  //  config.MaxPacketSize = Config_params.MaxPacketSize;
    config.DestAddress = hexToPlainString(Config_params.DestAddress);
    config.SourceAddress = hexToPlainString(Config_params.SourceAddress);
    
    Config_burst config_burst;
    config_burst.BurstSize = Config_params.BurstSize;
    config_burst.BurstPeriodicity_us = Config_params.BurstPeriodicity_us;
    config_burst.PacketGenerationTime_us = 1;

    std::vector<uint8_t> packet = generatePacket(config);
    generateBurstsToFileWithTiming(config_burst, "burst.txt", packet, Config_params.CaptureSizeMs);

    std::cout << "Generated Packet: ";
    printHex(packet);

    writeToFile("result.txt", packet);

    return 0;
}
