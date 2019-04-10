#include "Unpacker.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using json = nlohmann::json;

int main(int argc, char** argv) {
    // Default mapping.
    std::vector<int> map = {-1, 1, -3, 3};
    bool complex = true;
    unsigned int channels = 1;
    unsigned int bits = 2;
    string file = "";

    vector<string> channelFilenames;
    vector<ofstream> outFiles;
    vector<vector<int8_t>> outputBuffers;
    const unsigned int bufferSize = 1024;

    if (argc < 2) {
        cout << "Usage: unpacker [name of input json file with all settings]"
             << std::endl;
        return 0;
    }

    try {
        /**
         * Process the settings file by importing the settings file into JSON.
         */
        std::ifstream infile;
        infile.open(argv[1], ifstream::in);
        json settings;
        infile >> settings;
        bits = settings["bits"];
        channels = settings["channels"];
        file = settings["file"];
        for (auto iter = settings["out"].begin(); iter != settings["out"].end();
             iter++) {
            channelFilenames.push_back(*iter);
        }
        if (settings.find("mapping") != settings.end()) {
            cout << "Using user specified mapping." << endl;
            map.clear();
            for (auto iter = settings["mapping"].begin();
                 iter != settings["mapping"].end(); iter++) {
                map.push_back(*iter);
            }
        }
        infile.close();

        // Debug: Display results;
        std::cout << "Input file: " << file << endl;
        std::cout << "Channels " << channels << endl
                  << "Bits " << bits << endl
                  << "Complex " << complex << endl;
        std::cout << "Map:" << endl;
        for (auto i = 0; i < 4; i++) {
            std::cout << i << ": " << (signed)map[i] << std::endl;
        }

        //   Create unpacker.
        Unpacker<int8_t> unpacker(map, channels, bits, true);

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
        float lastPrint = 0;
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
            float percentage = (float)total / (float)fileSize;
            if ((percentage - lastPrint) >= 0.10 || percentage >= 0.98) {
                std::cout << total << " bytes processed, " << percentage * 100
                          << " percent complete." << std::endl;
                lastPrint = percentage;
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
