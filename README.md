Here's a description of the `.cpp` files and how they integrate, along with the logic behind them, for the README file:

---

# Ethernet Packet Generator

## Project Overview

This project focuses on generating Ethernet packets with specific configurations such as payloads, eCPRI and ORAN headers, IQ samples, and Inter-Frame Gaps (IFGs). It handles burst generation, packet fragmentation based on maximum packet size, and writing the generated packets into text files for further use.

## File Descriptions

### `configreader.cpp`
This file is responsible for reading and parsing the configuration parameters from an input file. It loads values such as line rate, burst size, destination and source MAC addresses, maximum packet size, and other Ethernet settings into a `Config_params` struct. These values are used throughout the project to dynamically generate Ethernet packets based on user-specified configurations.

### `iq_samples_reader.cpp`
This module handles the reading and processing of IQ sample data from a file. The samples are expected to be represented as pairs of I and Q values. These IQ samples are used in the ORAN payload of the Ethernet packet. The samples are read into a vector, and later this data is used in generating ORAN-specific packet headers.

### `payload.cpp`
This file generates the ORAN payload based on the ORAN headers and IQ samples. The ORAN header includes parameters such as frame ID, subframe ID, slot ID, symbol ID, and section ID, while the IQ samples are read and appended to the payload. The ORAN header and payload are integrated into the final Ethernet packet.

### `packetgenerator.cpp`
This file implements the Ethernet packet generation logic. It uses configuration values from `configreader.cpp` and payload data from `payload.cpp` to create an Ethernet frame. The packet consists of several components, including:
- Preamble
- Destination and Source MAC addresses
- EtherType
- Payload (eCPRI and ORAN headers with IQ samples)
- CRC-32 checksum

The function also appends Inter-Frame Gaps (IFGs) as bytes to ensure proper alignment during transmission.

### `BurstGenerator.cpp`
This file generates bursts of packets based on the provided burst size and line rate. It handles the timing and organization of how multiple packets are generated and grouped within a capture window. The bursts are written into text files after being generated.

### `test.cpp`
This is the main driver file of the project. It integrates all the functionalities from the above `.cpp` files. The logic flow begins with reading the configuration file and IQ samples, followed by generating ORAN headers, creating Ethernet packets, and finally writing these packets into a text file. The project also ensures proper 4-byte alignment and applies packet fragmentation when the size exceeds the configured maximum packet size.

## Integration and Logic

The project is structured in a modular way where each `.cpp` file handles a specific part of the packet generation process. The main program (`test.cpp`) coordinates the flow:
1. It reads configuration parameters and IQ samples.
2. It generates the ORAN and eCPRI headers.
3. It combines these headers with the payload to create an Ethernet frame.
4. It generates a burst of packets based on the configuration and writes them to an output file with proper handling of Inter-Frame Gaps (IFGs) to maintain alignment and timing.

---
