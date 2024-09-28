#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>

std::vector<std::pair<int16_t, int16_t>> readIQSamples(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::pair<int16_t, int16_t>> iqSamples;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return iqSamples;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int16_t i, q;

        // Read I and Q as signed 16-bit integers
        if (iss >> i >> q) {
            iqSamples.push_back({i, q});
        }
    }

    file.close();
    return iqSamples;
}
