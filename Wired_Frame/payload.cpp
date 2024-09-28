#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

// Define the eCPRI header structure (as before)
struct eCPRIHeader {
    uint8_t reserved;     // 1 byte, reserved (set to 0)
    uint8_t messageType;  // 1 byte, eCPRI message type (0x00 for user plane)
    uint16_t payloadSize; // 2 bytes, size of ORAN payload Size
    uint16_t pc_rtc;      // 2 bytes, fixed (0x00)
    uint16_t seqId;       // 2 bytes, sequence ID (increments with each packet)

    eCPRIHeader() : reserved(0x00), messageType(0x00), payloadSize(0), pc_rtc(0x00), seqId(0) {}
};

// Define the ORAN header structure
struct ORANHeader {
    uint8_t Datadirection_payloadversion_filterindex; // 1 byte
    uint8_t frameid; // 1 byte
    uint8_t subframeid; // 1 byte
    uint8_t slotid; // 1 byte
    uint8_t symbolid; // 1 byte
    uint8_t startprbu; // 1 byte
    uint8_t numprbu; // 1 byte
    
    ORANHeader() : Datadirection_payloadversion_filterindex(0x00), frameid(0x00), subframeid(0x00),slotid(0x00), symbolid(0x00), 
                   startprbu(0x00), numprbu(0x00) {} 

};

// Function to print bytes in hex format
void print_Hex(const std::vector<uint8_t>& bytes) {
    std::cout << std::hex << std::setfill('0');
    for (auto byte : bytes) {
        std::cout << std::setw(2) << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl; // Switch back to decimal format
}

// Function to generate the eCPRI header
std::vector<uint8_t> generateEcpriHeader(const eCPRIHeader& header) {
    std::vector<uint8_t> ecpriHeader;

    ecpriHeader.push_back(header.reserved);  // 1 byte
    ecpriHeader.push_back(header.messageType);  // 1 byte

    // Split 2-byte payload size into individual bytes
    ecpriHeader.push_back((header.payloadSize >> 8) & 0xFF);  // High byte
    ecpriHeader.push_back(header.payloadSize & 0xFF);          // Low byte

    // Split 2-byte pc_rtc into individual bytes
    ecpriHeader.push_back((header.pc_rtc >> 8) & 0xFF);  // High byte
    ecpriHeader.push_back(header.pc_rtc & 0xFF);         // Low byte

    // Split 2-byte seqId into individual bytes
    ecpriHeader.push_back((header.seqId >> 8) & 0xFF);  // High byte
    ecpriHeader.push_back(header.seqId & 0xFF);         // Low byte

    return ecpriHeader;
}

// Function to generate the ORAN header
std::vector<uint8_t> generateOranPacket(const ORANHeader& oranHeader, const std::vector<std::pair<int16_t, int16_t>>& iqSamples, size_t &numSamples,size_t&index) {
    std::vector<uint8_t> oranPacket;

    // Add the ORAN header
    oranPacket.push_back(oranHeader.Datadirection_payloadversion_filterindex);    
    oranPacket.push_back(oranHeader.frameid);          
    oranPacket.push_back(oranHeader.subframeid);
    oranPacket.push_back(oranHeader.slotid);       
    oranPacket.push_back(oranHeader.symbolid); 
    oranPacket.push_back(oranHeader.startprbu);
    oranPacket.push_back(oranHeader.numprbu);

    size_t endIndex =index + numSamples;
   // std::cout << "index: " << index << " endIndex: " << endIndex << std::endl;
    // Append IQ samples (up to numSamples) to the ORAN packet
    for (size_t i = index ; i < endIndex; ++i) {
        int16_t I = iqSamples[i].first;
        int16_t Q = iqSamples[i].second;

        // Split I (16-bit) into two 8-bit bytes and append them
        oranPacket.push_back(static_cast<uint8_t>((I >> 8) & 0xFF));  // High byte of I
        oranPacket.push_back(static_cast<uint8_t>(I & 0xFF));         // Low byte of I

        // Split Q (16-bit) into two 8-bit bytes and append them
        oranPacket.push_back(static_cast<uint8_t>((Q >> 8) & 0xFF));  // High byte of Q
        oranPacket.push_back(static_cast<uint8_t>(Q & 0xFF));         // Low byte of Q

    }
    index +=numSamples;
    return oranPacket;
}
