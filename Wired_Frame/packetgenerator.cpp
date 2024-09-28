#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <random>

struct Config_pkt {
    double LineRate = 0;
    int CaptureSizeMs = 0;
    int MinNumOfIFGsPerPacket = 0;
    std::string DestAddress;
    std::string SourceAddress;
    int MaxPacketSize = 0;
};

void printHex(const std::vector<uint8_t>& bytes) {
    std::cout << std::hex << std::setfill('0');
    for (auto byte : bytes) {
        std::cout << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl; 
}
uint32_t calculateCrc32(const std::vector<uint8_t>& data) {
    uint32_t crc = 0xFFFFFFFF; // Initialize with all bits set
    const uint32_t polynomial = 0xEDB88320;

    for (uint8_t byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc; 
}

    //Random Payload
    std::vector<uint8_t> generateRandomPayload(size_t payloadSize) {
    std::vector<uint8_t> payload(payloadSize);

    std::random_device rd;   // Random device for seeding
    std::mt19937 gen(rd());  // Mersenne Twister random number generator
    std::uniform_int_distribution<uint8_t> dist(0, 255); // Distribution range [0, 255]

    for (size_t i = 0; i < payloadSize; ++i) {
        payload[i] = dist(gen);
    }

    return payload;
}
//Ethernet Packet Generator
std::vector<uint8_t> generatePacket(const Config_pkt& config,const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> packet;

    // Add Preamble
    uint8_t preamble[] = {0xFB, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xD5}; // 8 bytes
    packet.insert(packet.end(), std::begin(preamble), std::end(preamble));

    // Add Destination Address (6 bytes)
    std::string destAddress = config.DestAddress;
    for (size_t i = 0; i < destAddress.length(); i += 2) {
        std::string byteString = destAddress.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        packet.push_back(byte);
    }

    // Add Source Address (6 bytes)
    std::string sourceAddress = config.SourceAddress;
    for (size_t i = 0; i < sourceAddress.length(); i += 2) {
        std::string byteString = sourceAddress.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        packet.push_back(byte);
    }
    // Add EtherType (2 bytes)
    uint16_t etherType = 0x07FF; // Example EtherType (0x07FF)
    packet.push_back(etherType & 0xFF);         // Byte 0 (LSB)
    packet.push_back((etherType >> 8) & 0xFF);  // Byte 1 (MSB)

    packet.insert(packet.end(), payload.begin(), payload.end());


    // Calculate CRC-32 for the packet so far
    uint32_t crc = calculateCrc32(packet);

    // Append the CRC-32 (4 bytes, least significant byte first)
    packet.push_back(crc & 0xFF);         // Byte 0 (LSB)
    packet.push_back((crc >> 8) & 0xFF);  // Byte 1
    packet.push_back((crc >> 16) & 0xFF); // Byte 2
    packet.push_back((crc >> 24) & 0xFF); // Byte 3 (MSB)

    // Ensure packet size is aligned to 4 bytes (before adding IFGs)
    size_t padding = (4 - (packet.size() % 4)) % 4;
    if (padding > 0) {
        std::vector<uint8_t> ifg(padding, 0x07); // IFG byte (0x07) padding
        packet.insert(packet.end(), ifg.begin(), ifg.end());
    }

    return packet;
}