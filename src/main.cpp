#include "Unpacker.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    // Default mapping.
    std::vector<int> map = {1, 3, -3, -1};
    bool complex = true;
    unsigned int channels = 2;
    unsigned int bits = 2;
    string file = "";

    vector<string> channelFilenames;
    vector<ofstream> outFiles;
    vector<vector<int8_t>> outputBuffers;
    const unsigned int bufferSize = 1024000;

    try {
        // Process the settings file.
        std::ifstream infile;
        infile.open("settings.txt", ifstream::in);
        while (!infile.eof()) {
            string key;
            string value;
            string value2;
            infile >> key;
            if (key == "bits") {
                infile >> value;
                bits = stoul(value);
            }
            if (key == "channels") {
                infile >> value;
                channels = stoul(value);
            }
            if (key == "map") {
                infile >> value;
                infile >> value2;
            }
            if (key == "file") {
                infile >> file;
                cout << "Input: " << file << std::endl;
            }
            if (key == "out") {
                infile >> value;
                channelFilenames.push_back(value);
                std::cout << "Added " << value << " for output " << std::endl;
            }
        }
        infile.close();

        // Debug: Display results;
        std::cout << "Channels " << channels << endl
                  << "Bits " << bits << endl
                  << "Complex " << complex << endl;
        std::cout << "Map:" << endl;
        for (auto i = 0; i < 4; i++) {
            std::cout << i << ": " << (signed)map[i] << std::endl;
        }

        //   Create unpacker.
        Unpacker unpacker(map, channels, bits, true);

        // Verify number of output files is correct.
        if (channelFilenames.size() != channels) {
            throw std::runtime_error(
                "Must specify the same number of channels as output filenames");
        }

        // Open output files and buffers.
        outFiles.resize(channels);
        outputBuffers.resize(channels);
        unsigned int outIdx = 0;
        for (auto iter = channelFilenames.begin();
             iter != channelFilenames.end(); iter++) {
            outFiles[outIdx].open((*iter).c_str(),
                                  std::ofstream::binary | std::ofstream::out);
            if (!outFiles[outIdx].is_open()) {
                throw std::runtime_error("Could not open output file.");
            }
            outputBuffers[outIdx].resize(
                unpacker.GetOutputBufferSize(bufferSize));
            outIdx++;
        }

        // Open the input file.
        infile.open(file, ifstream::in | ifstream::binary);
        if (!infile.is_open()) {
            throw std::runtime_error(
                "Error opening file. Does not exist or cannot be opened.");
        }

        // Create read buffer and unpacker
        std::vector<uint8_t> readBuffer;
        readBuffer.resize(bufferSize);

        // Perform unpacking
        unsigned int total = 0;
        unsigned int lastPrint = 0;
        infile.seekg(0, std::ios_base::end);
        unsigned int fileSize = infile.tellg();
        infile.seekg(0, std::ios_base::beg);
        while (!infile.eof()) {
            // read in the bytes;
            infile.read((char*)&readBuffer[0], bufferSize);

            // Unpack.
            unpacker.Unpack(readBuffer, outputBuffers);
            total += infile.gcount();

            // Write the data to files.
            auto index = 0;
            for (auto it = outFiles.begin(); it != outFiles.end(); it++) {
                (*it).write((char*)outputBuffers[index].data(),
                            outputBuffers[index].size());
                index++;
            }
            // Percentage
            if (total - lastPrint > 1000000000) {
                std::cout << total << " bytes processed, "
                          << (double)total / (double)fileSize
                          << " percent complete." << std::endl;
                lastPrint = total;
            }
        }

        // Cleanup
        infile.close();
        for (auto iter = outFiles.begin(); iter != outFiles.end(); iter++) {
            (*iter).close();
        }
    } catch (exception& e) {
        cout << "Exception: " << e.what() << endl;
        return -1;
    }
}
