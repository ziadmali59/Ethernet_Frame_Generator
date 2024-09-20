// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "configreader.cpp" // Include the configreader.cpp file

int main() {
    // File paths
    std::string configFilePath = "input.txt";  // Path to the configuration file
    std::string outputFilePath = "output.txt"; // Path to the output file

    // Open the input file
    std::ifstream inputFile(configFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file: " << configFilePath << std::endl;
        return 1;
    }

    // Open the output file
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << outputFilePath << std::endl;
        return 1;
    }

    // Load configuration
    Config_params config = loadConfig(configFilePath);

    // Verify if the configuration was loaded correctly
    if (config.LineRate == 0 && config.CaptureSizeMs == 0 && config.BurstSize == 0) {
        std::cerr << "Error: Configuration loading failed or no valid data found." << std::endl;
        return 1;
    }

    // Print the configuration to the terminal
    std::cout << "Configuration read from " << configFilePath << ":" << std::endl;
    printConfig(config, std::cout);

    // Dump the configuration to the output file
    outputFile << "Configuration read from " << configFilePath << ":" << std::endl;
    printConfig(config, outputFile);

    // Close the files
    inputFile.close();
    outputFile.close();

    std::cout << "Configuration written to " << outputFilePath << std::endl;

    return 0;
}
