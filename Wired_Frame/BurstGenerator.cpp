#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip> 

struct Config_burst {
    int BurstSize;               // Number of packets in one burst
    long long BurstPeriodicity_us; // Periodicity of bursts in microseconds
    int PacketGenerationTime_us;  // Time taken to generate one packet (mock)
};

void writeBytesInHex(std::ofstream& outputFile, const std::vector<uint8_t>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        outputFile << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
        if ((i + 1) % 4 == 0) {
            outputFile << '\n'; 
        }
    }
    outputFile << '\n'; 
}

int calculateNumberOfIFGs(const Config_burst& config, int totalBurstTime_us) {
    // Calculate how much time remains after sending the burst
    int remainingTime_us = config.BurstPeriodicity_us - totalBurstTime_us;

    // Each IFG represents 8 bytes (64 bits) sent at a time.
    // Calculate how many 64-bit IFGs can fit into the remaining time.
    int ifgCount = remainingTime_us / config.PacketGenerationTime_us;

    return ifgCount;
}

// Function to generate bursts of packets with timing control
void generateBurstsToFileWithTiming(const Config_burst& config, const std::string& filePath, const std::vector<uint8_t>& packet, int captureDurationMs) 
{
    std::ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << filePath << std::endl;
        return;
    }

    std::cout << "Starting burst generation with timing and writing to file..." << std::endl;

    auto overallStartTime = std::chrono::steady_clock::now(); // Track overall streaming time

    while (true) {
        // Check if total streaming duration is exceeded
        auto currentTime = std::chrono::steady_clock::now();
        int elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - overallStartTime).count();
        if (elapsedTimeMs >= captureDurationMs) {
            std::cout << "Capture duration exceeded. Stopping packet generation..." << std::endl;
            break; // Exit the loop once the streaming time has been reached
        }

        // Track time within a burst
        auto burstStartTime = std::chrono::steady_clock::now();

        // Generate Burst
        std::cout << "Generating a burst of " << config.BurstSize << " packets..." << std::endl;
        for (int i = 0; i < config.BurstSize; ++i) {
            outputFile << "Packet " << (i + 1) << ":\n";
            writeBytesInHex(outputFile, packet);
        }

        auto burstEndTime = std::chrono::steady_clock::now();
        int totalBurstTime_us = std::chrono::duration_cast<std::chrono::microseconds>(burstEndTime - burstStartTime).count();

        // Calculate IFGs based on remaining time in the burst period
        int numberOfIFGs = calculateNumberOfIFGs(config, totalBurstTime_us);

        // Write IFGs (zeros) to the file
        std::cout << "Inserting " << numberOfIFGs << " IFGs (zeros) to fill the gap..." << std::endl;
        for (int i = 0; i < numberOfIFGs; ++i) {
            std::vector<uint8_t> zeros(4, 0x00); // 4 zeros for alignment
            writeBytesInHex(outputFile, zeros);
        }

        std::cout << "Burst sent and written to file, waiting for next period..." << std::endl;

        // Sleep for the remaining time in the burst period
        auto burstTimeElapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - burstStartTime).count();
        std::this_thread::sleep_for(std::chrono::microseconds(config.BurstPeriodicity_us - burstTimeElapsed_us));
    }

    outputFile.close();
}

