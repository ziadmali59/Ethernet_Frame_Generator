#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include "payload.cpp"
#include "configreader.cpp"
#include "iq_samples_reader.cpp"


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
    // read configuration file
    // File paths
    std::string samplesfile = "iq_samples.txt";  
    std::string configFilePath = "input.txt";  // Path to the configuration file
    std::string outputFilePath = "output.txt"; // Path to the output file
    // Open the input file
    std::ifstream inputFile(configFilePath);
    std::ifstream inputFile2(samplesfile);
    // Open the output file
    std::ofstream outputFile(outputFilePath);
    // Load configuration
    Config_params Config_params = loadConfig(configFilePath);
    // Load IQ samples
    std::vector<std::pair<int16_t, int16_t>> iqSamples = readIQSamples(samplesfile);
     inputFile.close();
     inputFile2.close();
     outputFile.close();


    // Step 1: Generate the eCPRI Header
    eCPRIHeader ecpriHeader;
    ecpriHeader.seqId = 0x0302;  // Example: Set initial sequence ID
    ecpriHeader.payloadSize = (Config_params.NrbPerPacket)*2*12*8; // Example: NRBp*12samples*2(I&Q)*8bits
    std::vector<uint8_t> ecpriHeaderBytes = generateEcpriHeader(ecpriHeader);

    std::cout << "eCPRI Header:" << std::endl;
    printHex(ecpriHeaderBytes);

    // Step 2: Generate the ORAN Header
    ORANHeader oranHeader;
    std::vector<uint8_t> oranBytes = generateOranPacket(oranHeader, iqSamples, 10);
    std::cout << "ORAN Packet:" << std::endl;
    printHex(oranBytes);

    // Combine eCPRI header and ORAN header into a single frame
    std::vector<uint8_t> ecpriFrame = ecpriHeaderBytes;
    ecpriFrame.insert(ecpriFrame.end(), oranBytes.begin(), oranBytes.end());
    // Print the combined eCPRI + ORAN frame
    std::cout << "Complete eCPRI + ORAN Frame:" << std::endl;
    printHex(ecpriFrame);
    return 0;
}
